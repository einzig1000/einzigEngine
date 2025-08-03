#include "GameManager.h"

GameManager::GameManager()
{
    map_ = std::make_unique<Map>();

    map_->LoadMap(0);
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