#pragma once


#include <xaudio2.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <vector>
#include <map>
#include <string>
#include <atomic>
#include <sdkddkver.h>


// XAudio2ボイスイベント用のカスタムコールバック
class VoiceCallback : public IXAudio2VoiceCallback
{
public:
    // バッファの再生が終了したときに呼び出される
    STDMETHOD_(void, OnBufferEnd)(void* pBufferContext) override;
    // ボイスの処理パスが開始したときに呼び出される
    STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32 BytesRequired) override {}
    // ボイスの処理パスが終了したときに呼び出される
    STDMETHOD_(void, OnVoiceProcessingPassEnd)() override {}
    // ストリームが終了したときに呼び出される（ループ再生時等）
    STDMETHOD_(void, OnStreamEnd)() override {}
    // バッファの再生が開始したときに呼び出される
    STDMETHOD_(void, OnBufferStart)(void* pBufferContext) override {}
    // ループの終わりに達したときに呼び出される
    STDMETHOD_(void, OnLoopEnd)(void* pBufferContext) override {}
    // ボイスでエラーが発生したときに呼び出される
    STDMETHOD_(void, OnVoiceError)(void* pBufferContext, HRESULT Error) override {}
};

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();


    uint32_t LoadAudio(const std::string& filePath);

    // 読み込まれたオーディオを再生
    void PlayAudio(const uint32_t& audioId, bool loop);

    // 特定のオーディオの再生を停止
    void StopAudio(const uint32_t& audioId);

    // 特定のオーディオまたはマスターボリュームを設定
    void SetVolume(const uint32_t& audioId, float volume);
    void SetMasterVolume(float volume);

    // 特定のオーディオまたはマスターボリュームを返す
    float GetVolume(const uint32_t& audioId);
    float GetMasterVolume();

    // 現在再生してるか？
    bool IsAudioPlaying(const uint32_t& audioId);

private:
    // 初期化
    HRESULT Initialize();

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

    // デストラクタで何回もつかう
    void CleanupAudioEntry(AudioEntry& entry);

    // VoiceCallbackからAudioManagerへのアクセスを許可（フレンドクラスやファクトリパターンも検討）
    friend class VoiceCallback;
};