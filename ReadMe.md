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
   - 引数
      - ディレクトリパス
      - ファイルネーム
   - 返り値
      - モデルID
   - 備考
      - Game::LoadOBJ("resources/Prototypes/model/", "test.obj");のように使う
      
- uint32_t Game::LoadTexture(const std::string& filePath);
   - 引数
      - ディレクトリパス + ファイルネーム
   - 返り値
      - テクスチャID
   - 備考
      - Game::LoadTexture("resources/Prototypes/texture/test.png");のように使う
 
- uint32_t Game::LoadAudio(const std::string& filePath);
   - 引数
      - ディレクトリパス + ファイルネーム
   - 返り値
      - オーディオID
   - 備考
      - Game::LoadAudio("resources/Prototypes/audio/BGM/Morning.mp3");のように使う

- TextureData* Game::GetTexture(uint32_t textureNumber);
   - 引数
      - テクスチャID
   - 返り値
      - テクスチャIDに応じたmetadataを含むテクスチャの情報
   - 備考
      - ImGuiで画像描画したいからmetadata欲しい！って時とかに使う

## オーディオ系
- void Game::PlayAudio(const uint32_t& audioId, bool loop);
   - 引数
     - オーディオID
     - true の時はループ再生
   - 返り値
      - なし
   - 備考
      - オーディオの再生
      
- void Game::StopAudio(const uint32_t& audioId);
   - 引数
     - オーディオID
   - 返り値
      - なし
   - 備考
      - オーディオの停止
  
- void Game::SetAudioVolume(const uint32_t& audioId, float volume);
   - 引数
     - オーディオID
     - 音量
   - 返り値
      - なし
   - 備考
      - オーディオの音量の設定
        
- void Game::SetMasterVolume(float volume);
   - 引数
     - 音量
   - 返り値
      - なし
   - 備考
      - マスター音量の設定

- uint32_t Game::GetVolume(const uint32_t& audioId);
   - 引数
     - オーディオID
   - 返り値
      - オーディオの音量
   - 備考
      - 
  
- void Game::GetMasterVolume();
   - 引数
      - なし
   - 返り値
      - マスターの音量
   - 備考
      - 

- bool Game::IsAudioPlaying(const uint32_t& audioId);
   - 引数
      - オーディオID
   - 返り値
      - オーディオが再生されているか
   - 備考
      - 
  

## ライト系
- Game::SetLightColor(const Vector4 color);
   - 引数
      - 
   - 返り値
      - オーディオが再生されているか
   - 備考
      - 
 

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
