#include "GameManager.h"

GameManager::GameManager()
{
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
    // マップ
    map_->Draw();


}