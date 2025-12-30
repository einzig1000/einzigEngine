#include "GameScenePhase.h"
#include "MapManager/MapManager.h"
#include "Charactor/Player/Player.h"
#include "Camera/CameraController.h"
#include "UIManager/UIManager.h"
#include <fstream>

GameScenePhase::GameScenePhase()
{
	// プレイヤー生成
	player_ = std::make_unique<Player>();
	// マップマネージャー生成
	map_ = std::make_unique<MapManager>(player_.get());
	// カメラコントローラー生成
	cameraController_ = std::make_unique<CameraController>();
	// UIマネージャー生成
	uiManager_ = std::make_unique<UIManager>(player_.get());

	// カメラコントローラーにプレイヤーとマップマネージャーをセット
	cameraController_->SetPlayer(player_.get());
	cameraController_->SetMapManager(map_.get());

	// プレイヤーにマップマネージャーをセット
	player_->SetMapManager(map_.get());

	map_->LoadMap("resources/Map/map.json");
}

GameScenePhase::~GameScenePhase() {}

void GameScenePhase::Initialize()
{
	nextPhase_ = PHASE::Phase_None;

	map_->Initialize();
	player_->Initialize();
	uiManager_->Initialize();


	Game::Camera::SetCameraMode(CameraMode_ORBIT_FPS::FPS);
	Game::Input::Mouse::ShowCursor(false);
}


void GameScenePhase::Update()
{
	// プレイヤー更新
	player_->Update();
	// マップ更新
	map_->Update();
	// UI更新
	uiManager_->Update();
	// カメラ更新
	cameraController_->Update();

	if (Game::Input::Key::IsJustPressed(DIK_R))
	{
		Game::Input::Mouse::ToggleMouseCursorVisible();
	}
}


void GameScenePhase::Draw()
{
	// マップ描画
	map_->Draw();
	// プレイヤー描画
	player_->Draw();
	// UI描画
	uiManager_->Draw();
}

void GameScenePhase::DrawImGui()
{
	// マップImGui描画
	map_->DrawImGui();
	// プレイヤーImGui描画
	player_->DrawImGui();
	// UIImGui描画
	uiManager_->DrawImGui();
}




