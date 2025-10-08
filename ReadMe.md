### workflow states badge

[![DebugBuild](https://github.com/einzig1000/CG2/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/einzig1000/CG2/actions/workflows/DebugBuild.yml)

### 機能
## メインループ
- - [x] Game::Initialize(int width, int height, const std::wstring& title);
  - (int ウィンドウ横幅, int ウィンドウ縦幅, const std::wstring& タイトル)
  - ウィンドウのサイズとウィンドウの名前を設定できる。

- -[]Game::ProcessMessage()
- Game::BeginFrame()
- Game::UpdateTransforms();
- Game::EndFrame();
- Game::Finalize();

## ロード系
- Game::LoadOBJ(const std::string& directoryPath, const std::string& filename)
- Game::LoadTexture(const std::string& filePath);
- Game::LoadAudio(const std::string& filePath);
- Game::GetTexture(uint32_t textureNumber);

## オーディオ系
- Game::PlayAudio(const uint32_t& audioId, bool loop);
- Game::StopAudio(const uint32_t& audioId);
- Game::SetAudioVolume(const uint32_t& audioId, float volume);
- Game::SetMasterVolume(float volume);
- Game::GetVolume(const uint32_t& audioId);
- Game::GetMasterVolume();
- Game::IsAudioPlaying(const uint32_t& audioId);

## ライト系
- Game::SetLightColor(const Vector4 color);
- Game::SetLightDirection(const Vector3 direction);
- Game::SetLightIntensity(float intensity);
- Game::ToggleLightMode(const uint32_t mode);

## 入力系
- Game::GetMousePosition();
- Game::GetMouseRay();
- Game::IsCollisionMouseRayAABB(uint32_t objectNumber, const Transforms& data);
- Game::GetMousePress(int i);
- Game::GetMousePrePress(int i)
- Game::GetMouseWheel();

## カメラ系
- Game::MoveCenterTarget(Vector3 target, int spendFrame, EaseType easetype);
- Game::MoveRotateTarget(Vector3 target, int spendFrame, EaseType easetype);
- Game::MoveDistanceTarget(float target, int spendFrame, EaseType easetype);
- Game::SetControlModeCamera(bool mode);
- Game::GetCamera();
- Game::GetDebugCamera();

## 
- Game::toggleWireframeMode();
