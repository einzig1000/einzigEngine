#include "GameScenePhase.h"
#include "MapManager.h"
#include "Player.h"
#include "CameraController.h"
#include <fstream>

GameScenePhase::GameScenePhase()
{
	player_ = std::make_unique<Player>();

	map_ = std::make_unique<MapManager>(player_.get());

	player_->SetMapManager(map_.get());

	cameraController_ = std::make_unique<CameraController>(player_.get());


	map_->LoadMap("resources/Map/map.csv");
}

GameScenePhase::~GameScenePhase() {}

void GameScenePhase::Initialize()
{
	nextPhase_ = PHASE::Phase_None;

	map_->Initialize();
	player_->Initialize();


	Game::Camera::SetCameraMode(CameraMode_ORBIT_FPS::FPS);
	Game::Input::Mouse::ShowCursor(false);
}


void GameScenePhase::Update()
{
	map_->Update();

	player_->Update();

	cameraController_->Update();

	if (Game::Input::Key::IsJustPressed(DIK_R))
	{
		Game::Input::Mouse::ToggleMouseCursorVisible();
	}
}


void GameScenePhase::Draw()
{
	map_->Draw();

	player_->Draw();
}

void GameScenePhase::DrawImGui()
{
	map_->DrawImGui();
}




