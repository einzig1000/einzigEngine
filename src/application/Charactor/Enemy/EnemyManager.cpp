#include "Charactor/Enemy/EnemyManager.h"
#include "Charactor/Enemy/Enemy.h"
#include "Charactor/Player/Player.h"
#include "MapManager/MapManager.h"


EnemyManager::EnemyManager()
{}

EnemyManager::~EnemyManager()
{}

void EnemyManager::Initialize()
{}

void EnemyManager::Update()
{
	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}
}

void EnemyManager::Draw()
{
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}
}

void EnemyManager::DrawImGui()
{
	for (auto& enemy : enemies_)
	{
		enemy->DrawImGui();
	}
}

void EnemyManager::AddNewEnemy(Vector3 pos)
{
	auto enemy = std::make_unique<Enemy>(pos);
	enemy->SetPlayerPtr(playerptr_);
	enemy->SetMapManager(mapManager_);
	enemy->SetUIManager(uiManager_);
	enemy->Initialize();
	mapManager_->RegisterCharactor(enemy.get());
	enemies_.emplace_back(std::move(enemy));
}
