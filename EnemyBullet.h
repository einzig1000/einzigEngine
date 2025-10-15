#pragma once
#include "Game.h"

class Player;

namespace
{
	// 弾用アセットを一度だけロード
	uint32_t s_bulletModel = UINT32_MAX;
	uint32_t s_bulletTexture = UINT32_MAX;
	void EnsureBulletAssets()
	{
		if (s_bulletModel == UINT32_MAX)
		{
			s_bulletModel = Game::LoadOBJ("resources/Prototypes/model/", "sphere.obj");
		}
		if (s_bulletTexture == UINT32_MAX)
		{
			s_bulletTexture = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
		}
	}
}

class EnemyBullet
{
public:
	EnemyBullet(Vector3 spown, Vector3 velocity);
	~EnemyBullet();
	// コピーでRenderData_Modelの登録/解除が壊れないように禁止
	EnemyBullet(const EnemyBullet&) = delete;
	EnemyBullet& operator=(const EnemyBullet&) = delete;
	EnemyBullet(EnemyBullet&&) = delete;
	EnemyBullet& operator=(EnemyBullet&&) = delete;

	void Update(Player& player);
	void Draw();

	Game::RenderData_Model data;


	//static uint32_t tex;// = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	//static uint32_t model;// = Game::LoadOBJ("resources/Prototypes/model/", "sphere.obj");
};

