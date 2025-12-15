#include "GameScenePhase.h"
#include "MapManager.h"
#include "Player.h"
#include <fstream>

GameScenePhase::GameScenePhase()
{
	player_ = std::make_unique<Player>();

	map_ = std::make_unique<MapManager>(player_.get());


	//map_->LoadMap("resources/Map/map1x1.csv");
	map_->LoadMap("resources/Map/map.csv");
	//map_->LoadMap("resources/Map/mapFlat.csv");
	//map_->LoadMap("resources/Map/map140x140.csv");
}

GameScenePhase::~GameScenePhase() {}

void GameScenePhase::Initialize()
{
	nextPhase_ = PHASE::Phase_None;

	map_->Initialize();
	player_->Initialize();

	Game::Camera::SetCameraMode(CameraMode::FPS);
	Game::Input::Mouse::ShowCursor(false);
}


void GameScenePhase::Update()
{
	map_->Update();

	player_->Update();
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




