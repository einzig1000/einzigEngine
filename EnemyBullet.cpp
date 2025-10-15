#include "EnemyBullet.h"
#include "Player.h"

//uint32_t EnemyBullet::tex = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
//uint32_t EnemyBullet::model = Game::LoadOBJ("resources/Prototypes/model/", "sphere.obj");

EnemyBullet::EnemyBullet(Vector3 spown, Vector3 velocity)
{
	EnsureBulletAssets();
	data.transforms.translate = spown;
	data.velocity = velocity;
	data.model = s_bulletModel;
	data.texture = s_bulletTexture;
	data.color = 0xFF0000FF;
	data.transforms.scale = { 0.2f,0.2f,0.2f };
}

EnemyBullet::~EnemyBullet()
{}

void EnemyBullet::Update(Player& player)
{
	if (data.isCollision(player.bullet_->data))
	{
		// 衝突したら消える
		data.transforms.translate = { 0.0f, -100.0f, 0.0f };
	}
}

void EnemyBullet::Draw()
{
	data.Draw();
}