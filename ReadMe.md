### workflow states badge

[![DebugBuild](https://github.com/einzig1000/CG2/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/einzig1000/CG2/actions/workflows/DebugBuild.yml)

### 機能
## メインループ
- - [x] Game::Initialize(int width, int height, const std::wstring& title);
   (int ウィンドウ横幅, int ウィンドウ縦幅, const std::wstring& タイトル)
   ウィンドウのサイズとウィンドウの名前を設定できる。
- - [x] Game::ProcessMessage()
- - [x] Game::BeginFrame()
- - [x] Game::UpdateTransforms();
- - [x] Game::EndFrame();
- - [x] Game::Finalize();

## ロード系
- uint32_t Game::LoadOBJ(const std::string& directoryPath, const std::string& filename)
  引数
     - ディレクトリパス
     - ファイルネーム)
  返り値
     - モデルID
  備考
     - Game::LoadOBJ("resources/Prototypes/model/", "test.obj");のように使う
- uint32_t Game::LoadTexture(const std::string& filePath);
  (const std::string& ディレクトリパス + ファイルネーム)
  Game::LoadTexture("resources/Prototypes/texture/test.png");のように使う
- uint32_t Game::LoadAudio(const std::string& filePath);
  (const std::string& ディレクトリパス + ファイルネーム)
  Game::LoadTexture("resources/Prototypes/audio/BGM/Morning.mp3");のように使う
- TextureData* Game::GetTexture(uint32_t textureNumber);
  (uint32_t Game::LoadAudioの返り値(オーディオID))
  ImGuiで画像描画したいからmetadata欲しい！って時とかに使う

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
