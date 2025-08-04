#include "GameManager.h"

GameManager::GameManager()
{
	// ブロック
	Game::LoadOBJ("resources/block", "map.obj");
	Game::LoadTexture("resources/block/map.png");
	// 駒
	Game::LoadOBJ("resources/Charactor/king", "king.obj");
	Game::LoadTexture("resources/Charactor/king/king.png");


	// フェーズ管理用
	requestPhase_ = Phase::Title;


    map_ = new Map();

    map_->LoadMap(0);
}

GameManager::~GameManager()
{
    delete map_;
    map_ = nullptr;
}

void GameManager::Update()
{
	if (requestPhase_ != Phase::None)
	{
		switch (requestPhase_)
		{
		case Phase::None:
			break;
		case Phase::Title:
			phase_ = Phase::Title;
			break;
		case Phase::ActSelect:
			phase_ = Phase::ActSelect;
			break;
		case Phase::StageSelect:
			phase_ = Phase::StageSelect;
			break;
		case Phase::Game:
			phase_ = Phase::Game;
			break;
		case Phase::UnitOverview:
			phase_ = Phase::UnitOverview;
			break;
		default:
			break;
		}

		requestPhase_ = Phase::None;
	}

    map_->Update();
}

void GameManager::Draw()
{
    map_->Draw();
}