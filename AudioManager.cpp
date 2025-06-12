#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib") // 新規追加: UUID定義のため、これも必要になる場合があります

#include "AudioManager.h"
#include <combaseapi.h> // CoInitializeEx, CoUninitialize のため
#include <iostream>     // 基本的なエラー出力のため（任意、デバッグ用）
#include <Windows.h>    // MultiByteToWideChar, WideCharToMultiByte のため
#include <wrl/client.h> // Microsoft::WRL::ComPtr のため
#include <iomanip>

// Media Foundation Headers
#include <mfapi.h>      // MFStartup, MFShutdown など基本的なAPI
#include <mfidl.h>      // IMFSourceReader, MF_MD_DURATION などインターフェースや属性キー
#include <mfreadwrite.h> // MFCreateSourceReaderFromURL などリーダー/ライターAPI

// utilities for PROPVARIANT
#include <propvarutil.h> // PropVariantInit, PropVariantClear のため (念のため追加)

#include "functions.h" // Log関数などの定義があるはず



// VoiceCallbackの実装
void VoiceCallback::OnBufferEnd(void* pBufferContext)
{
    // ストリーミング再生の場合、ここで次のバッファをサブミットするロジックを実装します。
    // この例ではファイル全体がメモリに読み込まれるため、このコールバックは
    // 主に必要に応じてループ再生を制御するために使用できます。
    // 例: pBufferContext に AudioEntry* が渡される場合、そこから再生状態を管理できます。
}

// AudioManagerの実装
AudioManager::AudioManager()
    : pXAudio2(nullptr), pMasteringVoice(nullptr)
{}

AudioManager::~AudioManager()
{
    Deinitialize(); // shutdown から Deinitialize に変更
}

HRESULT AudioManager::Initialize()
{
    // XAudio2の初期化
    HRESULT hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr))
    {
        std::cerr << "エラー: XAudio2エンジンの作成に失敗しました。HRESULT: " << std::hex << hr << std::endl;
        return hr;
    }

    // マスタリングボイスの作成
    hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice);
    if (FAILED(hr))
    {
        std::cerr << "エラー: XAudio2マスタリングボイスの作成に失敗しました。HRESULT: " << std::hex << hr << std::endl;
        if (pXAudio2) pXAudio2->Release();
        pXAudio2 = nullptr;
        return hr;
    }

    // Media Foundationの初期化
    // Media Foundation APIを使用する前にMFStartupを呼び出す必要があります
    hr = MFStartup(MF_VERSION, MFSTARTUP_FULL);
    if (FAILED(hr))
    {
        std::cerr << "エラー: Media Foundationの初期化に失敗しました。HRESULT: " << std::hex << hr << std::endl;
        if (pMasteringVoice) pMasteringVoice->DestroyVoice();
        if (pXAudio2) pXAudio2->Release();
        pMasteringVoice = nullptr;
        pXAudio2 = nullptr;
        return hr;
    }

    return S_OK;
}

void AudioManager::Deinitialize() // shutdown から Deinitialize に変更
{
    // 読み込まれた全てのオーディオエントリをクリーンアップします
    for (auto& pair : loadedAudio)
    {
        CleanupAudioEntry(pair.second);
    }
    loadedAudio.clear();

    // マスタリングボイスを破棄します
    if (pMasteringVoice)
    {
        pMasteringVoice->DestroyVoice();
        pMasteringVoice = nullptr;
    }
    // XAudio2エンジンを解放します
    if (pXAudio2)
    {
        pXAudio2->Release();
        pXAudio2 = nullptr;
    }

    // Media Foundationをシャットダウンします
    // Media Foundation APIが不要になったらMFShutdownを呼び出す必要があります
    MFShutdown();
}

//uint32_t AudioManager::LoadAudio(const std::string& filePath)
//{
//    // 既にaudioIdが設定されている
//    if (loadedAudio.count(audioId))
//    {
//        std::cerr << "警告: ID '" << filePath << "' のオーディオは既に読み込まれています。" << std::endl;
//        return S_FALSE;
//    }
//
//    AudioEntry newEntry = {};
//    HRESULT hr = ReadAudioData(filePath, newEntry);
//    // シンプルに読み込めなかった
//    if (FAILED(hr))
//    {
//        std::cerr << "エラー: '" << filePath << "' からオーディオデータを読み込めませんでした。HRESULT: " << std::hex << hr << std::endl;
//        CleanupAudioEntry(newEntry);
//        return hr;
//    }
//    // ReadAudioData 関数の最後に追加
//
//    // newEntry.pSourceVoiceにソースボイスオブジェクトのアドレスを入れるらしい
//    hr = pXAudio2->CreateSourceVoice(&newEntry.pSourceVoice, &newEntry.wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &voiceCallback);
//    if (FAILED(hr))
//    {
//        std::cerr << "エラー: ID '" << filePath << "' のXAudio2ソースボイスを作成できませんでした。HRESULT: " << std::hex << hr << std::endl;
//        CleanupAudioEntry(newEntry);
//        return hr;
//    }
//
//    // XAUDIO2_BUFFERを設定？
//    newEntry.xAudioBuffer.pAudioData = newEntry.audioData.data();
//    newEntry.xAudioBuffer.AudioBytes = static_cast<UINT32>(newEntry.audioData.size());
//    newEntry.xAudioBuffer.Flags = XAUDIO2_END_OF_STREAM; // これが最後のバッファであることを示します
//    newEntry.audioId = loadedAudio.size();
//
//    loadedAudio[audioId] = newEntry;
//    std::cout << "情報: オーディオ '" << audioId << "' を正常に読み込みました。" << std::endl;
//    return newEntry.audioId;
//}

uint32_t AudioManager::LoadAudio(const std::string& filePath)
{
    static uint32_t nextAudioId = 0;
    AudioEntry entry = {};
    HRESULT hr = S_OK;

    // ファイルパスをワイド文字列に変換
    int wlen = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, nullptr, 0);
    std::wstring wFilePath(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, &wFilePath[0], wlen);

    Microsoft::WRL::ComPtr<IMFSourceReader> pSourceReader;

    // ソースリーダーの作成
    hr = MFCreateSourceReaderFromURL(wFilePath.c_str(), nullptr, &pSourceReader);
    if (FAILED(hr))
    {
        Log("Failed to create source reader from URL:", hr);
        return UINT32_MAX; // エラー
    }

    // オーディオストリームを選択
    hr = pSourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
    if (FAILED(hr))
    {
        Log("Failed to select audio stream:", hr);
        return UINT32_MAX;
    }

    // オーディオフォーマットの取得と設定
    Microsoft::WRL::ComPtr<IMFMediaType> pMediaType;
    hr = pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMediaType);
    if (FAILED(hr))
    {
        Log("Failed to get current media type:", hr);
        return UINT32_MAX;
    }

    // WAVEFORMATEX情報を取得
    UINT32 formatSize = 0;
    WAVEFORMATEX* wfx = nullptr;
    hr = MFCreateWaveFormatExFromMFMediaType(pMediaType.Get(), &wfx, &formatSize);
    if (FAILED(hr))
    {
        Log("Failed to create WAVEFORMATEX from media type:", hr);
        return UINT32_MAX;
    }
    memcpy(&entry.wfx, wfx, sizeof(WAVEFORMATEX));
    CoTaskMemFree(wfx); // 取得したメモリを解放

    Log("--- WAVEFORMATEX Debug Info ---");
    Log("wFormatTag", entry.wfx.wFormatTag);
    Log("nChannels", entry.wfx.nChannels);
    Log("nSamplesPerSec", entry.wfx.nSamplesPerSec);
    Log("nAvgBytesPerSec", entry.wfx.nAvgBytesPerSec);
    Log("nBlockAlign", entry.wfx.nBlockAlign);
    Log("wBitsPerSample", entry.wfx.wBitsPerSample);
    Log("cbSize", entry.wfx.cbSize);
    Log("-------------------------------");

    // オーディオの総バイト数を推定
    // MF_MD_DURATION (プレゼンテーションの期間) を取得し、nAvgBytesPerSec を用いて計算
    PROPVARIANT var;
    PropVariantInit(&var);
    hr = pSourceReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_MD_DURATION, &var);
    LONGLONG duration = 0; // 単位は100ナノ秒
    if (SUCCEEDED(hr))
    {
        duration = var.uhVal.QuadPart;
        PropVariantClear(&var);
    }
    else
    {
        Log("Failed to get audio duration. This might affect expected byte calculation.");
    }

    // 推定されるオーディオデータの総バイト数
    // duration は 100ナノ秒単位なので、秒に変換するために 10,000,000 で割る
    // その後、秒 * nAvgBytesPerSec で推定される総バイト数を計算
    LONGLONG estimatedTotalAudioBytes = 0;
    if (entry.wfx.nAvgBytesPerSec > 0)
    { // 0除算防止
        estimatedTotalAudioBytes = (duration / 10000000LL) * entry.wfx.nAvgBytesPerSec;
        // 小数点以下の秒数を考慮したより正確な計算
        estimatedTotalAudioBytes += ((duration % 10000000LL) * entry.wfx.nAvgBytesPerSec) / 10000000LL;
    }
    Log("Estimated Total Audio Bytes (approx):", estimatedTotalAudioBytes);


    // オーディオデータの読み込み
    DWORD currentBufferLength = 0;
    DWORD totalAudioDataSize = 0; // 実際に読み込んだ総バイト数を追跡
    entry.audioData.reserve(static_cast<size_t>(estimatedTotalAudioBytes > 0 ? estimatedTotalAudioBytes : 1024 * 1024)); // 事前にメモリを確保 (推定値またはデフォルトで1MB)

    while (true)
    {
        Microsoft::WRL::ComPtr<IMFSample> pSample;
        hr = pSourceReader->ReadSample(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            0,
            nullptr,            // actual timestamp
            nullptr,            // actual stream flags
            nullptr,            // actual system time
            &pSample
        );

        if (FAILED(hr))
        {
            Log("ReadSample FAILED with HRESULT:", hr);
            break;
        }

        if (pSample == nullptr)
        {
            Log("End of stream reached or pSample is null.");
            break;
        }

        Microsoft::WRL::ComPtr<IMFMediaBuffer> pBuffer;
        hr = pSample->ConvertToContiguousBuffer(&pBuffer);
        if (FAILED(hr))
        {
            Log("ConvertToContiguousBuffer FAILED with HRESULT:", hr);
            break;
        }

        BYTE* pAudioData = nullptr;
        hr = pBuffer->Lock(&pAudioData, nullptr, &currentBufferLength);
        if (FAILED(hr))
        {
            Log("Buffer Lock FAILED with HRESULT:", hr);
            break;
        }

        if (currentBufferLength > 0)
        {
            size_t current_vector_size = entry.audioData.size();
            entry.audioData.resize(current_vector_size + currentBufferLength);
            memcpy(entry.audioData.data() + current_vector_size, pAudioData, currentBufferLength);
            totalAudioDataSize += currentBufferLength; // 読み込んだバイト数を加算
        }

        hr = pBuffer->Unlock();
        if (FAILED(hr))
        {
            Log("Buffer Unlock FAILED with HRESULT:", hr);
            break;
        }

        // currentBufferLength == 0 はストリームの終端を示す
        if (currentBufferLength == 0)
        {
            Log("currentBufferLength is 0. End of audio data.");
            break;
        }
    }

    Log("Actual total audio data loaded:", totalAudioDataSize);
    Log("Final audioData.size():", entry.audioData.size()); // totalAudioDataSize と同じになるはず

    if (FAILED(hr) && hr != S_OK) // ループがエラーで終了した場合は無効なIDを返す
    {
        Log("Audio data loading failed during read loop.");
        return UINT32_MAX;
    }

    // XAudio2Bufferの設定
    ZeroMemory(&entry.xAudioBuffer, sizeof(entry.xAudioBuffer));
    entry.xAudioBuffer.AudioBytes = (UINT32)entry.audioData.size();
    entry.xAudioBuffer.pAudioData = entry.audioData.data();
    entry.xAudioBuffer.LoopBegin = 0;
    entry.xAudioBuffer.LoopLength = 0;
    entry.xAudioBuffer.LoopCount = 0; // 0は1回再生

    // ソースボイスの作成
    hr = pXAudio2->CreateSourceVoice(&entry.pSourceVoice, &entry.wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &voiceCallback);
    if (FAILED(hr))
    {
        Log("Failed to create source voice:", hr);
        return UINT32_MAX;
    }

    // マップに格納
    uint32_t id = nextAudioId++;
    loadedAudio[id] = std::move(entry); // move で効率的に格納
    Log("Audio loaded successfully. ID:", id, " Path:", filePath);

    return id;
}

void AudioManager::PlayAudio(const uint32_t& audioId, bool loop)
{
    auto it = loadedAudio.find(audioId);
    if (it == loadedAudio.end())
    {
        std::cerr << "エラー: ID '" << audioId << "' のオーディオは読み込まれていません。" << std::endl;
        assert(0);
    }

    AudioEntry& entry = it->second;
    entry.xAudioBuffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;

    // すでに再生中の場合は、再度サブミットする前に停止してバッファをフラッシュします
    entry.pSourceVoice->Stop(0);
    entry.pSourceVoice->FlushSourceBuffers();

    HRESULT hr = entry.pSourceVoice->SubmitSourceBuffer(&entry.xAudioBuffer);
    if (FAILED(hr))
    {
        std::cerr << "エラー: ID '" << audioId << "' のオーディオバッファのサブミットに失敗しました。HRESULT: " << std::hex << hr << std::endl;
        assert(0);
    }

    hr = entry.pSourceVoice->Start(0);
    if (FAILED(hr))
    {
        std::cerr << "エラー: ID '" << audioId << "' のオーディオ再生の開始に失敗しました。HRESULT: " << std::hex << hr << std::endl;
    }
    else
    {
        std::cout << "情報: オーディオ '" << audioId << "' を再生しています (ループ: " << (loop ? "True" : "False") << ")。" << std::endl;
    }
}

void AudioManager::StopAudio(const uint32_t& audioId)
{
    auto it = loadedAudio.find(audioId);
    if (it != loadedAudio.end())
    {
        it->second.pSourceVoice->Stop(0);
        it->second.pSourceVoice->FlushSourceBuffers(); // 保留中のバッファをクリア
        std::cout << "情報: オーディオ '" << audioId << "' を停止しました。" << std::endl;
    }
    else
    {
        std::cerr << "警告: 読み込まれていないオーディオID '" << audioId << "' の停止を試みました。" << std::endl;
    }
}

void AudioManager::SetVolume(const uint32_t& audioId, float volume)
{
    auto it = loadedAudio.find(audioId);
    if (it != loadedAudio.end())
    {
        it->second.pSourceVoice->SetVolume(volume);
        std::cout << "情報: オーディオ '" << audioId << "' のボリュームを " << volume << " に設定しました。" << std::endl;
    }
    else
    {
        std::cerr << "警告: 読み込まれていないオーディオID '" << audioId << "' のボリューム設定を試みました。" << std::endl;
    }
}

void AudioManager::SetMasterVolume(float volume)
{
    if (pMasteringVoice)
    {
        pMasteringVoice->SetVolume(volume);
        std::cout << "情報: マスターボリュームを " << volume << " に設定しました。" << std::endl;
    }
    else
    {
        std::cerr << "警告: マスターボイスが初期化されていません。マスターボリュームを設定できません。" << std::endl;
    }
}

// Media Foundation を使用したオーディオデータ読み込みの基本実装
// goto を削除し、Microsoft::WRL::ComPtr を使用してリソースを自動管理
HRESULT AudioManager::ReadAudioData(const std::string& filePath, AudioEntry& entry)
{
    HRESULT hr = S_OK;

    Microsoft::WRL::ComPtr<IMFSourceReader> pReader;
    Microsoft::WRL::ComPtr<IMFMediaType> pMediaType;
    Microsoft::WRL::ComPtr<IMFMediaType> pPartialType;
    Microsoft::WRL::ComPtr<IMFSample> pSample;

    // URL（ファイルパス）からソースリーダーを作成
    hr = MFCreateSourceReaderFromURL(ConvertString(filePath).c_str(), nullptr, &pReader);
    if (FAILED(hr)) return hr;

    // 全てのストリームを選択解除
    hr = pReader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
    if (FAILED(hr)) return hr;

    // 最初のオーディオストリームを選択
    hr = pReader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
    if (FAILED(hr)) return hr;

    // オーディオストリームの現在のメディアタイプを取得
    hr = pReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMediaType);
    if (FAILED(hr)) return hr;

    GUID majorType;
    hr = pMediaType->GetMajorType(&majorType);
    if (FAILED(hr)) return hr;

    // メインタイプがオーディオでない場合はエラー
    if (majorType != MFMediaType_Audio)
    {
        return MF_E_INVALIDMEDIATYPE;
    }

    // デコーダをPCM出力に設定
    hr = MFCreateMediaType(&pPartialType);
    if (FAILED(hr)) return hr;

    hr = pPartialType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    if (FAILED(hr)) return hr;

    hr = pPartialType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM); // PCM形式でデコード
    if (FAILED(hr)) return hr;

    hr = pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pPartialType.Get());
    if (FAILED(hr)) return hr;

    // 変換後の実際のPCMメディアタイプを取得
    pMediaType.Reset(); // 既存のpMediaTypeを解放
    hr = pReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMediaType);
    if (FAILED(hr)) return hr;

    // WAVEFORMATEX 構造体に変換
    UINT32 cbFormat = 0;
    WAVEFORMATEX* wf = nullptr;
    hr = MFCreateWaveFormatExFromMFMediaType(pMediaType.Get(), &wf, &cbFormat);
    if (FAILED(hr)) return hr;

    entry.wfx = *wf;
    CoTaskMemFree(wf); // CoTaskMemAllocで割り当てられたメモリを解放

    // 全てのサンプルを読み込む
    DWORD actualStreamIndex = 0;
    DWORD flags = 0;
    LONGLONG timestamp = 0;

    while (true)
    {
        // 多分ここでデコード
        hr = pReader->ReadSample(
            MF_SOURCE_READER_FIRST_AUDIO_STREAM, // dwStreamIndex (DWORD)
            0,                                   // dwControlFlags (DWORD)
            &actualStreamIndex,                  // pdwActualStreamIndex (DWORD*) - 実際に読み込まれたストリームのインデックスを受け取る
            &flags,                              // pdwStreamFlags (DWORD*) - ストリームフラグを受け取る
            &timestamp,                          // pllTimestamp (LONGLONG*) - タイムスタンプを受け取る
            pSample.ReleaseAndGetAddressOf()     // ppSample (IMFSample** ) - サンプルを受け取る
        );

        if (FAILED(hr))
        {
            //break; // エラー発生時に中断
            assert(0);
        }
        // デコード＆entryに入力が終わった
        if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            break; // ストリームの終わりに達した
        }

        // pSampleあれば
        if (pSample)
        {
            Microsoft::WRL::ComPtr<IMFMediaBuffer> pBuffer;
            hr = pSample->ConvertToContiguousBuffer(&pBuffer); // 連続したメモリバッファに変換
            if (FAILED(hr)) break;

            // オーディオデータを１～１０とした時、そして１～３で１つのチャンクだった時
            BYTE* pAudioBytes = nullptr;
            DWORD cbAudioBytes = 0;
            // ここでpAudioBytesには１のポインタが渡される、cbAudioBytesは３（１チャンクのサイズ）が与えられる
            hr = pBuffer->Lock(&pAudioBytes, nullptr, &cbAudioBytes); // バッファをロックしてデータにアクセス
            if (FAILED(hr)) break;

            // オーディオデータをベクターに追加
            // ここでentry.audioDataに「entry.audioData.end()」に「pAudioBytes」から「pAudioBytes + cbAudioBytes」バイト分入れる
            entry.audioData.insert(entry.audioData.end(), pAudioBytes, pAudioBytes + cbAudioBytes);
            hr = pBuffer->Unlock(); // バッファのロック解除 (ComPtrが自動解放するため、Unlock後にReleaseは不要)
        }
        pSample.Reset(); // 次のループのためにサンプルを解放
    }


    Log("--- WAVEFORMATEX Debug Info ---");
    Log("wFormatTag", entry.wfx.wFormatTag);
    Log("nChannels", entry.wfx.nChannels);
    Log("nSamplesPerSec", entry.wfx.nSamplesPerSec);
    Log("nAvgBytesPerSec", entry.wfx.nAvgBytesPerSec);
    Log("nBlockAlign", entry.wfx.nBlockAlign);
    Log("wBitsPerSample", entry.wfx.wBitsPerSample);
    Log("cbSize", entry.wfx.cbSize);
    Log("audioData.size()", entry.audioData.size());
    Log("-------------------------------");

    return hr; // ループ内でエラーが発生した場合も hr が返される
}

void AudioManager::CleanupAudioEntry(AudioEntry& entry)
{
    if (entry.pSourceVoice)
    {
        entry.pSourceVoice->DestroyVoice();
        entry.pSourceVoice = nullptr;
    }
    // audioData は std::vector なので、自動的に解放されます
    // xAudioBuffer.pAudioData は audioData を指しているため、別途解放は不要です
}