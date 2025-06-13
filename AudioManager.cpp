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


// x
AudioManager::AudioManager()
    : pXAudio2(nullptr), pMasteringVoice(nullptr)
{
    Initialize();
}

// x
AudioManager::~AudioManager()
{
    // 読み込まれた全てのオーディオエントリをクリーンアップ
    for (auto& pair : loadedAudio)
    {
        CleanupAudioEntry(pair.second);
    }
    loadedAudio.clear();

    // マスタリングボイスを破棄
    if (pMasteringVoice)
    {
        pMasteringVoice->DestroyVoice();
        pMasteringVoice = nullptr;
    }
    // XAudio2エンジンを解放
    if (pXAudio2)
    {
        pXAudio2->Release();
        pXAudio2 = nullptr;
    }

    MFShutdown();
    CoUninitialize();
}

// x
HRESULT AudioManager::Initialize()
{
    // XAudio2の初期化
    HRESULT hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr))
    {
        Log("XAudio2エンジンの作成に失敗しました。HRESULT: 0x%X", hr);
        assert(0);
    }

    // マスタリングボイスの作成
    hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice);
    if (FAILED(hr))
    {
        Log("XAudio2マスタリングボイスの作成に失敗しました。HRESULT: 0x%X", hr);
        if (pXAudio2) pXAudio2->Release();
        pXAudio2 = nullptr;
        assert(0);
    }

    // Media Foundationの初期化
    // Media Foundation APIを使用する前にMFStartupを呼び出す必要があります
    hr = MFStartup(MF_VERSION, MFSTARTUP_FULL);
    if (FAILED(hr))
    {
        Log("Media Foundationの初期化に失敗しました。HRESULT: 0x%X", hr);
        if (pMasteringVoice) pMasteringVoice->DestroyVoice();
        if (pXAudio2) pXAudio2->Release();
        pMasteringVoice = nullptr;
        pXAudio2 = nullptr;
        assert(0);
    }

    return S_OK;
}

// 読み込み
uint32_t AudioManager::LoadAudio(const std::string& filePath)
{
    static uint32_t nextAudioId = 0;
    AudioEntry entry = {};
    HRESULT hr = S_OK;

    // ファイルパスをワイド文字列に変換
    std::wstring wFilePath = ConvertString(filePath);

    Microsoft::WRL::ComPtr<IMFSourceReader> pSourceReader;

    // ソースリーダー(オーディオデータを読み取るためのインターフェース)の作成
    hr = MFCreateSourceReaderFromURL(wFilePath.c_str(), nullptr, &pSourceReader);
    if (FAILED(hr))
    {
        Log("ソースリーダーの作成に失敗しました: 0x%X", hr);
        assert(0);
    }

    // メディアファイルには 複数のストリーム（音声・動画・字幕など） が含まれていることがあるため音声を取得するよと設定しているらしい
    hr = pSourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
    if (FAILED(hr))
    {
        Log("取得ストリームの設定に失敗しました: 0x%X", hr);
        assert(0);
    }

    // 音声データがどんな形式(MP3,WAV,AACとか)で保存されているかを調べる
    Microsoft::WRL::ComPtr<IMFMediaType> pMediaType;
    hr = pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMediaType);
    if (FAILED(hr))
    {
        Log("オーディオフォーマットの取得に失敗しました: 0x%X", hr);
        assert(0);
    }

    // 上で取得した形式からWAVEFORMATEXに変換
    UINT32 formatSize = 0;
    WAVEFORMATEX* wfx = nullptr;
    hr = MFCreateWaveFormatExFromMFMediaType(pMediaType.Get(), &wfx, &formatSize);
    if (FAILED(hr))
    {
        Log("メディアタイプの変換に失敗しました: 0x%X", hr);
        return UINT32_MAX;
    }
    memcpy(&entry.wfx, wfx, sizeof(WAVEFORMATEX));
    CoTaskMemFree(wfx); // 取得したメモリを解放

    Log("--- WAVEFORMATEX Debug Info ---");
    Log("wFormatTag: 0x%X", entry.wfx.wFormatTag);
    Log("nChannels: %u", entry.wfx.nChannels);
    Log("nSamplesPerSec: %u", entry.wfx.nSamplesPerSec);
    Log("nAvgBytesPerSec: %u", entry.wfx.nAvgBytesPerSec);
    Log("nBlockAlign: %u", entry.wfx.nBlockAlign);
    Log("wBitsPerSample: %u", entry.wfx.wBitsPerSample);
    Log("cbSize: %u", entry.wfx.cbSize);
    Log("-------------------------------");

    // オーディオデータの読み込み
    DWORD currentBufferLength = 0;
    DWORD totalAudioDataSize = 0; // 実際に読み込んだ総バイト数を追跡

    // 初期化時に適度な容量を予約 (例えば、1MB)。後で必要に応じて拡張される
    // audioData.size() が250400とのことなので、それを目安に少し余裕を持たせる
    entry.audioData.reserve(256 * 1024); // 256 KB を予約

    while (true)
    {
        Microsoft::WRL::ComPtr<IMFSample> pSample;
        DWORD actualStreamIndex = 0;
        DWORD streamFlags = 0;
        LONGLONG timestamp = 0;

        hr = pSourceReader->ReadSample(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            0,
            &actualStreamIndex,
            &streamFlags,
            &timestamp,
            &pSample
        );

        if (FAILED(hr))
        {
            Log("ReadSample FAILED with HRESULT: 0x%X", hr);
            break;
        }

        if (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            Log("End of stream reached.");
            break;
        }

        if (pSample == nullptr)
        {
            Log("pSample is null but not end of stream. This might be unexpected.");
            break;
        }

        Microsoft::WRL::ComPtr<IMFMediaBuffer> pBuffer;
        hr = pSample->ConvertToContiguousBuffer(&pBuffer);
        if (FAILED(hr))
        {
            Log("ConvertToContiguousBuffer FAILED with HRESULT: 0x%X", hr);
            break;
        }

        BYTE* pAudioData = nullptr;
        hr = pBuffer->Lock(&pAudioData, nullptr, &currentBufferLength);
        if (FAILED(hr))
        {
            Log("Buffer Lock FAILED with HRESULT: 0x%X", hr);
            break;
        }

        if (currentBufferLength > 0)
        {
            // audioData の末尾にデータを追加
            size_t current_vector_size = entry.audioData.size();
            entry.audioData.resize(current_vector_size + currentBufferLength);
            memcpy(entry.audioData.data() + current_vector_size, pAudioData, currentBufferLength);
            totalAudioDataSize += currentBufferLength;
            Log("Read chunk: %u bytes. Total: %u bytes.", currentBufferLength, totalAudioDataSize);
        }

        hr = pBuffer->Unlock();
        if (FAILED(hr))
        {
            Log("Buffer Unlock FAILED with HRESULT: 0x%X", hr);
            break;
        }
    }

    Log("Actual total audio data loaded: %u", totalAudioDataSize);
    Log("Final audioData.size(): %u", (uint32_t)entry.audioData.size());


    if (hr != S_OK && hr != MF_E_END_OF_STREAM)
    {
        Log("Audio data loading encountered an unhandled error. HRESULT: 0x%X", hr);
        return UINT32_MAX;
    }

    // XAudio2Bufferの設定
    ZeroMemory(&entry.xAudioBuffer, sizeof(entry.xAudioBuffer));
    // ここが非常に重要: 読み込んだバイト数をそのまま設定
    entry.xAudioBuffer.AudioBytes = totalAudioDataSize; // entry.audioData.size() でも同じはず
    entry.xAudioBuffer.pAudioData = entry.audioData.data();
    entry.xAudioBuffer.LoopBegin = 0;
    entry.xAudioBuffer.LoopLength = 0;
    entry.xAudioBuffer.LoopCount = 0; // 0は1回再生

    // ソースボイスの作成
    // XAudio2Create が成功し、pXAudio2 が有効であることを確認
    if (!pXAudio2)
    {
        Log("XAudio2 engine not initialized when trying to create source voice.");
        return UINT32_MAX;
    }

    hr = pXAudio2->CreateSourceVoice(&entry.pSourceVoice, &entry.wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &voiceCallback);
    if (FAILED(hr))
    {
        Log("Failed to create source voice: 0x%X", hr);
        return UINT32_MAX;
    }

    // マップに格納
    uint32_t id = nextAudioId++;
    loadedAudio[id] = std::move(entry);
    Log("Audio loaded successfully. ID: %u Path: %s", id, filePath.c_str());

    return id;
}

// 再生
void AudioManager::PlayAudio(const uint32_t& audioId, bool loop)
{
    auto it = loadedAudio.find(audioId);
    if (it != loadedAudio.end())
    {
        AudioEntry& entry = it->second;

        // すでに再生中であれば停止
        entry.pSourceVoice->Stop(0);
        entry.pSourceVoice->FlushSourceBuffers(); // バッファをクリア

        // loopが１の場合XAUDIO2_LOOP_INFINITE(無限ループ)に設定。0なら0
        entry.xAudioBuffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0; // ループ設定

        // オーディオデータをキューに送信し再生の準備をする(サブミット)
        HRESULT hr = entry.pSourceVoice->SubmitSourceBuffer(&entry.xAudioBuffer);
        if (FAILED(hr))
        {
            Log("Failed to submit source buffer for audio ID: %u HRESULT: 0x%X", audioId, hr);
            assert(0);
        }

        // 再生
        hr = entry.pSourceVoice->Start(0);
        if (FAILED(hr))
        {
            Log("%dのオーディオバッファの再生に失敗しました。: % u HRESULT : 0x % X", audioId, hr);
            assert(0);
        }
        Log("ID:%uのオーディオを再生します。Loop: %d", audioId, loop);
    }
    else
    {
        Log("存在しないオーディオの再生を失敗しました。ID: %u", audioId);
        assert(0);
    }
}

// 再生中のオーディオ停止
void AudioManager::StopAudio(const uint32_t& audioId)
{
    auto it = loadedAudio.find(audioId);
    if (it != loadedAudio.end())
    {
        it->second.pSourceVoice->Stop(0);
        it->second.pSourceVoice->FlushSourceBuffers();
        Log("%uのオーディオを停止しました", audioId);
    }
    else
    {
        Log("存在しないオーディオを停止しました。ID: %u", audioId);
        assert(0);
    }
}

// 特定のオーディオ音量のセッターゲッター
void AudioManager::SetVolume(const uint32_t& audioId, float volume)
{
    auto it = loadedAudio.find(audioId);
    // findできなかった場合はloadedAudio.end()がかえってくる。
    if (it != loadedAudio.end())
    {
        it->second.pSourceVoice->SetVolume(volume);
        Log("%uのボリュームを%fに設定しました", audioId, volume);
    }
    else
    {
        Log("存在しないオーディオのボリュームを設定しました。ID: %u", audioId);
        assert(0);
    }
}
float AudioManager::GetVolume(const uint32_t& audioId)
{
    auto it = loadedAudio.find(audioId);
    if (it != loadedAudio.end() && it->second.pSourceVoice)
    {
        float currentVolume = 0.0f;
        it->second.pSourceVoice->GetVolume(&currentVolume);
        return currentVolume;
    }
    Log("AudioID:%u が存在しないか意味分からんバグか", audioId);
    return 0.0f;
}

// マスター音量のセッターゲッター
void AudioManager::SetMasterVolume(float volume)
{
    if (pMasteringVoice)
    {
        pMasteringVoice->SetVolume(volume);
        Log("マスターボリューム: %f", volume);
    }
    else
    {
        Log("多分マスターボリュームが初期化されてないとかです");
        assert(0);
    }
}
float AudioManager::GetMasterVolume()
{
    if (pMasteringVoice)
    {
        float currentVolume = 0.0f;
        pMasteringVoice->GetVolume(&currentVolume);
        return currentVolume;
    }
    Log("マスター音量を取得できませんでした。なんてログがでることは100%ありえません");
    return 0.0f;
}

// 解放のループ内でたくさん使う
void AudioManager::CleanupAudioEntry(AudioEntry& entry)
{
    if (entry.pSourceVoice)
    {
        entry.pSourceVoice->DestroyVoice();
        entry.pSourceVoice = nullptr;
    }
}

// なんかいずれ使えるらしいけどまだ理解できない・
void VoiceCallback::OnBufferEnd(void* pBufferContext)
{

}
