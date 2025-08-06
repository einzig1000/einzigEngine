#include "GameScene.h"
#include "CharacterManager.h"

GameScene::GameScene(CharacterManager* characterManager)
    : characterManager_(characterManager)
{
    nextPhase = PHASE::Phase_None;

    map_ = new Map();
}

GameScene::~GameScene()
{
    delete map_;
    map_ = nullptr;
}

void GameScene::Initialize()
{
    nextPhase = PHASE::Phase_None;

    map_->LoadMap(1);

    Game::MoveCenterTarget({ -11.390f, -0.170f, -5.530f }, 0);
    Game::MoveDistanceTarget(35.60f, 0);
    Game::MoveRotateTarget({ 1.13f, 0.0f, 0.0f }, 0);
}

void GameScene::Update()
{
    map_->Update();
}

void GameScene::Draw()
{
    map_->Draw();

    // キャラの描画
    for (uint32_t i = 0; i < characterManager_->GetButtleCharactor().size(); ++i)
    {
        characterManager_->GetButtleCharactor()[i]->data.Draw();
    }
}