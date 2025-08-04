#include "GameManager.h"

GameManager::GameManager()
{
	// ブロック
	Game::LoadOBJ("resources/block", "map.obj");
	Game::LoadTexture("resources/block/map.png");
	// 駒
	Game::LoadOBJ("resources/Charactor/king", "king.obj");
	Game::LoadTexture("resources/Charactor/king/king.png");


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
    map_->Update();
}

void GameManager::Draw()
{
    map_->Draw();
}