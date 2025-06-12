#pragma once

#include <xaudio2.h>
#include <sdkddkver.h> // これが一番最初の方にあることを確認

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00 // Windows 10
#endif

#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <vector>
#include <map>
#include <string>
#include <atomic> // ストリーミング再生のための同期プリミティブ

// Windows 10 バージョン 1709 (Fall Creators Update) 以降を対象とします。
// 必要に応じて、プロジェクト設定で_WIN32_WINNTの値を調整してください。
// 例: #define _WIN32_WINNT _WIN32_WINNT_WIN10 // 最新のWindows 10
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00 // Windows 10
#endif

// XAudio2ボイスイベント用のカスタムコールバック
class VoiceCallback : public IXAudio2VoiceCallback
{
public:
    STDMETHOD_(void, OnBufferEnd)(void* pBufferContext) override;
    STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32 BytesRequired) override {}
    STDMETHOD_(void, OnVoiceProcessingPassEnd)() override {}
    STDMETHOD_(void, OnStreamEnd)() override {}
    STDMETHOD_(void, OnBufferStart)(void* pBufferContext) override {}
    STDMETHOD_(void, OnLoopEnd)(void* pBufferContext) override {}
    STDMETHOD_(void, OnVoiceError)(void* pBufferContext, HRESULT Error) override {}
};

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    // 初期化と終了処理
    HRESULT Initialize();
    void Deinitialize(); // shutdown から変更


    uint32_t LoadAudio(const std::string& filePath);

    // 読み込まれたオーディオを再生します
    void PlayAudio(const uint32_t& audioId, bool loop = false);

    // オーディオの再生を停止します
    void StopAudio(const uint32_t& audioId);

    // 特定のオーディオまたはマスターボリュームを設定します
    void SetVolume(const uint32_t& audioId, float volume);
    void SetMasterVolume(float volume);

private:
    IXAudio2* pXAudio2;
    IXAudio2MasteringVoice* pMasteringVoice;
    VoiceCallback voiceCallback;

    // オーディオデータとソースボイスを保持する構造体
    struct AudioEntry
    {
        IXAudio2SourceVoice* pSourceVoice;
        std::vector<BYTE> audioData;
        WAVEFORMATEX wfx;
        XAUDIO2_BUFFER xAudioBuffer;
        uint32_t audioId;
    };
    std::map<uint32_t, AudioEntry> loadedAudio;

    // Media Foundation を使用してオーディオデータを読み込みます
    HRESULT ReadAudioData(const std::string& filePath, AudioEntry& entry);
    // AudioEntryのリソースをクリーンアップします
    void CleanupAudioEntry(AudioEntry& entry);

    // VoiceCallbackからAudioManagerへのアクセスを許可（フレンドクラスやファクトリパターンも検討）
    friend class VoiceCallback;

    uint32_t audioId = 0;
};