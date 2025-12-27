#include "BaseCharactor.h"
#include "MapManager.h"
#include "Chunk.h"
#include "Block/Block.h"
#include "Block/BlockDurability.h"

// 見ているブロックをtargetBlock_にセットする
void BaseCharactor::SetTargetBlock()
{
	//// 視線レイ取得
	//Vector3 direction = Game::Math::DirectionFromYawPitch(data_.rotate.value.y, data_.rotate.value.x);
	//viewRay_.origin = data_.aabbs[0].center();
	//viewRay_.diff = direction.Normalized();

	// 前フレームの見ているブロック保存
	preTargetBlock_ = targetBlock_;
	// 見ているブロック取得
	targetBlock_ = mapManager_->IntersectRayBlock(viewRay_);
}


void BaseCharactor::SetMapManager(MapManager* mapManager)
{
	mapManager_ = mapManager;
}

void BaseCharactor::ResolveMapCollision()
{
#pragma region 下方向

	float playerHeight = data_.aabbs[0].max.y - data_.aabbs[0].min.y;

	Vector3 downCorners[4] = {
		Vector3(data_.aabbs[0].min.x, data_.aabbs[0].min.y - 0.1f, data_.aabbs[0].min.z),
		Vector3(data_.aabbs[0].max.x, data_.aabbs[0].min.y - 0.1f, data_.aabbs[0].min.z),
		Vector3(data_.aabbs[0].min.x, data_.aabbs[0].min.y - 0.1f, data_.aabbs[0].max.z),
		Vector3(data_.aabbs[0].max.x, data_.aabbs[0].min.y - 0.1f, data_.aabbs[0].max.z)
	};

	bool anyCollision = false;
	float bestGroundY = -999.9f;

	for (int i = 0; i < 4; ++i)
	{
		// 足元のブロックAABBを取得
		AABB blockAABB = mapManager_->GetAABB(downCorners[i]);

		if (
			// 足元のブロックAABBとキャラクターAABBが衝突していて
			IsCollision(blockAABB, data_.aabbs[0]) &&
			// そのブロックがアクティブなら
			mapManager_->GetIsActive(downCorners[i])
			)
		{
			// 衝突判定成立
			anyCollision = true;

			// 足元ブロックの上面Y座標を取得
			float groundY = blockAABB.max.y;

			// 一番高い地面Y座標を保存
			if (groundY > bestGroundY) bestGroundY = groundY;
		}
	}

	// 衝突していたら
	if (anyCollision)
	{
		// translate.value.y を地面上に設定する（translate が中心なら center.y = groundY + height/2）
		data_.translate.value.y = bestGroundY + playerHeight * 0.5f;

		// AABB を center から再計算するか、min/max を更新する
		data_.aabbs[0].min.y = data_.translate.value.y - playerHeight * 0.5f;
		data_.aabbs[0].max.y = data_.translate.value.y + playerHeight * 0.5f;

		data_.translate.velocity.y = 0.0f;
		data_.translate.acceleration.y = 0.0f;
	}
	// 空中にいたら
	else
	{
		data_.translate.acceleration.y = GRAVITY;
	}

#pragma endregion

#pragma region 左右方向

	float playerWidthX = data_.aabbs[0].max.x - data_.aabbs[0].min.x;

	Vector3 XplusCorners[4] = {
		Vector3(data_.aabbs[0].max.x + 0.1f, data_.aabbs[0].min.y, data_.aabbs[0].min.z),
		Vector3(data_.aabbs[0].max.x + 0.1f, data_.aabbs[0].min.y, data_.aabbs[0].max.z),
		Vector3(data_.aabbs[0].max.x + 0.1f, data_.aabbs[0].max.y, data_.aabbs[0].min.z),
		Vector3(data_.aabbs[0].max.x + 0.1f, data_.aabbs[0].max.y, data_.aabbs[0].max.z)
	};
	Vector3 XminusCorners[4] = {
		Vector3(data_.aabbs[0].min.x - 0.1f, data_.aabbs[0].min.y, data_.aabbs[0].min.z),
		Vector3(data_.aabbs[0].min.x - 0.1f, data_.aabbs[0].min.y, data_.aabbs[0].max.z),
		Vector3(data_.aabbs[0].min.x - 0.1f, data_.aabbs[0].max.y, data_.aabbs[0].min.z),
		Vector3(data_.aabbs[0].min.x - 0.1f, data_.aabbs[0].max.y, data_.aabbs[0].max.z)
	};



#pragma endregion

#pragma region 前後方向

	float playerWidthZ = data_.aabbs[0].max.z - data_.aabbs[0].min.z;


#pragma endregion


}

void BaseCharactor::Jump()
{
	data_.translate.velocity.y = jumpPower_;
	//data_.translate.acceleration.y = GRAVITY;
	data_.aabbs[0].min.y += jumpPower_;
	data_.aabbs[0].max.y += jumpPower_;
}

void BaseCharactor::Move(const Vector3& direction, float speed)
{
	data_.translate.velocity.x = direction.x * speed;
	data_.translate.velocity.z = direction.z * speed;

	data_.aabbs[0].min.x += data_.translate.velocity.x;
	data_.aabbs[0].max.x += data_.translate.velocity.x;
	data_.aabbs[0].min.z += data_.translate.velocity.z;
	data_.aabbs[0].max.z += data_.translate.velocity.z;
}


void BaseCharactor::BreakTargetBlock()
{
	if (targetBlock_.has_value())
	{
		lookAtBlock* lab = targetBlock_.value();
		if (lab != nullptr)
		{
			Block* block = lab->block;
			if (block != nullptr)
			{
				block->durability_->DecreaseDurability(breakPower_);
			}
		}
	}
}
