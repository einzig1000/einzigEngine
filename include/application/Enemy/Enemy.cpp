#include "Enemy.h"
#include "Player/Player.h"

Enemy::Enemy(int Tex, int model)
{
	data.texture = Tex;
	data.model = model;
	frame = 0;

	data.transforms.translate = { 0.0f, 0.0f, RandomFloat(20.0f,150.0f,1) };
}

Enemy::~Enemy()
{}


void Enemy::Update(Player& player)
{
	// 発射
	if (data.inPicture)
	{
		if (frame % 20 == 0)
		{
			Vector3 dir = (player.data.GetWorldPosition() - data.GetWorldPosition()).Normalized() * 0.3f;
			// ヒープに確保してポインタで保持（登録を維持）
			bulletList.emplace_back(std::make_unique<EnemyBullet>(data.GetWorldPosition(), dir));
		}
		data.LookAtOnce(player.data);
	}

	if (data.isCollision(player.bullet_->data))
	{
		// 衝突したら消える
		data.transforms.translate = { 0.0f, -100.0f, 0.0f };
	}
	if (data.isCollision(player.data))
	{
		player.攻撃終後の速度 = std::sqrtf(2.0f * player.data.gravity.y * player.落下高度);
	}

	frame++;

	for (auto& bullet : bulletList)
	{
		bullet->Update(player);
	}
}

void Enemy::Draw()
{
	data.Draw();

	for (auto& bullet : bulletList)
	{
		bullet->Draw();
	}
}
