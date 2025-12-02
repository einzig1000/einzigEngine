#include "GameScenePhase.h"
#include "MapManager.h"
#include "Player.h"
#include <fstream>

GameScenePhase::GameScenePhase()
{
	player_ = std::make_unique<Player>();

	map_ = std::make_unique<MapManager>(player_.get());

	particle_ = new RenderData_Particle();
	particle_->model = ResourceID::GetModelID(ModelID::Cube);
	particle_->texture = ResourceID::GetTextureID(TextureID::monsterBall);


	//map_->LoadMap("resources/Map/map1x1.csv");
	map_->LoadMap("resources/Map/map.csv");
	//map_->LoadMap("resources/Map/mapFlat.csv");
}

GameScenePhase::~GameScenePhase()
{
	delete particle_;
	particle_ = nullptr;	

	//delete map_;
	//map_ = nullptr;

	//delete player_;
	//player_ = nullptr;
}

void GameScenePhase::Initialize()
{
	nextPhase = PHASE::Phase_None;

	map_->Initialize();
	player_->Initialize();

	Game::Camera::SetCurrentOrbitMode(false);
	//Game::Input::Mouse::ToggleMouseCursorVisible();
}


void GameScenePhase::Update()
{
	map_->Update();

	player_->Update();
}


void GameScenePhase::Draw()
{
	map_->Draw();

	particle_->Draw();
	particle_->DrawEmitter();
	particle_->DrawImGui();

	player_->Draw();
}




