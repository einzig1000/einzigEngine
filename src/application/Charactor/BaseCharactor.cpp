#include "BaseCharactor.h"
#include "MapManager/MapManager.h"
#include "MapManager/Chunk/Chunk.h"
#include "MapManager/Chunk/Block/Block.h"
#include "MapManager/Chunk/Block/BlockDurability.h"

// 見ているブロックをtargetBlock_にセットする
void BaseCharactor::SetTargetBlock()
{
	// 前フレームの見ているブロック保存
	preTargetBlock_ = targetBlock_;
	// 見ているブロック取得
	targetBlock_ = mapManager_->GetBlockByCrossedRay(viewRay_, maxDistance);
}


void BaseCharactor::SetMapManager(MapManager* mapManager)
{
	mapManager_ = mapManager;
}

//void BaseCharactor::ResolveMapCollision()
//{
//#pragma region 下方向
//
//	float playerHeight = data_.aabbs[0].max.y - data_.aabbs[0].min.y;
//
//	Vector3 downCorners[4] = {
//		Vector3(data_.aabbs[0].min.x, data_.aabbs[0].min.y - 0.1f, data_.aabbs[0].min.z),
//		Vector3(data_.aabbs[0].max.x, data_.aabbs[0].min.y - 0.1f, data_.aabbs[0].min.z),
//		Vector3(data_.aabbs[0].min.x, data_.aabbs[0].min.y - 0.1f, data_.aabbs[0].max.z),
//		Vector3(data_.aabbs[0].max.x, data_.aabbs[0].min.y - 0.1f, data_.aabbs[0].max.z)
//	};
//
//	bool anyCollision = false;
//	float bestGroundY = -999.9f;
//
//	for (int i = 0; i < 4; ++i)
//	{
//		// 足元のブロックAABBを取得
//		AABB blockAABB = mapManager_->GetAABB(downCorners[i]);
//
//		if (
//			// 足元のブロックAABBとキャラクターAABBが衝突していて
//			IsCollision(blockAABB, data_.aabbs[0]) &&
//			// そのブロックがアクティブなら
//			mapManager_->GetIsActive(downCorners[i])
//			)
//		{
//			// 衝突判定成立
//			anyCollision = true;
//
//			// 足元ブロックの上面Y座標を取得
//			float groundY = blockAABB.max.y;
//
//			// 一番高い地面Y座標を保存
//			if (groundY > bestGroundY) bestGroundY = groundY;
//		}
//	}
//
//	// 衝突していたら
//	if (anyCollision)
//	{
//		// translate.value.y を地面上に設定する（translate が中心なら center.y = groundY + height/2）
//		data_.translate.value.y = bestGroundY + playerHeight * 0.5f;
//
//		// AABB を center から再計算するか、min/max を更新する
//		data_.aabbs[0].min.y = data_.translate.value.y - playerHeight * 0.5f;
//		data_.aabbs[0].max.y = data_.translate.value.y + playerHeight * 0.5f;
//
//		data_.translate.velocity.y = 0.0f;
//		data_.translate.acceleration.y = 0.0f;
//	}
//	// 空中にいたら
//	else
//	{
//		data_.translate.acceleration.y = GRAVITY;
//	}
//
//#pragma endregion
//
//#pragma region 左右方向
//
//	float playerWidthX = data_.aabbs[0].max.x - data_.aabbs[0].min.x;
//
//	Vector3 XplusCorners[4] = {
//		Vector3(data_.aabbs[0].max.x + 0.1f, data_.aabbs[0].min.y, data_.aabbs[0].min.z),
//		Vector3(data_.aabbs[0].max.x + 0.1f, data_.aabbs[0].min.y, data_.aabbs[0].max.z),
//		Vector3(data_.aabbs[0].max.x + 0.1f, data_.aabbs[0].max.y, data_.aabbs[0].min.z),
//		Vector3(data_.aabbs[0].max.x + 0.1f, data_.aabbs[0].max.y, data_.aabbs[0].max.z)
//	};
//	Vector3 XminusCorners[4] = {
//		Vector3(data_.aabbs[0].min.x - 0.1f, data_.aabbs[0].min.y, data_.aabbs[0].min.z),
//		Vector3(data_.aabbs[0].min.x - 0.1f, data_.aabbs[0].min.y, data_.aabbs[0].max.z),
//		Vector3(data_.aabbs[0].min.x - 0.1f, data_.aabbs[0].max.y, data_.aabbs[0].min.z),
//		Vector3(data_.aabbs[0].min.x - 0.1f, data_.aabbs[0].max.y, data_.aabbs[0].max.z)
//	};
//
//
//
//#pragma endregion
//
//#pragma region 前後方向
//
//	float playerWidthZ = data_.aabbs[0].max.z - data_.aabbs[0].min.z;
//
//
//#pragma endregion
//
//
//}

void BaseCharactor::ResolveMapCollision()
{
	if (!mapManager_) return;

	AABB& playerAabb = data_.aabbs[0];

	const float halfX = (playerAabb.max.x - playerAabb.min.x) * 0.5f;
	const float halfY = (playerAabb.max.y - playerAabb.min.y) * 0.5f;
	const float halfZ = (playerAabb.max.z - playerAabb.min.z) * 0.5f;

	const float probe = 0.1f;

#pragma region 下方向
	{
		// 壁際の誤判定を減らすため、少し内側をサンプルする
		const float inset = 0.02f;

		Vector3 downCorners[4] = {
			Vector3(playerAabb.min.x + inset, playerAabb.min.y - probe, playerAabb.min.z + inset),
			Vector3(playerAabb.max.x - inset, playerAabb.min.y - probe, playerAabb.min.z + inset),
			Vector3(playerAabb.min.x + inset, playerAabb.min.y - probe, playerAabb.max.z - inset),
			Vector3(playerAabb.max.x - inset, playerAabb.min.y - probe, playerAabb.max.z - inset)
		};

		bool anyCollision = false;
		float bestGroundY = -999999.0f;

		for (int i = 0; i < 4; ++i)
		{
			AABB blockAABB = mapManager_->GetAABB(downCorners[i]);

			if (mapManager_->GetIsActive(downCorners[i]) && IsCollision(blockAABB, playerAabb))
			{
				anyCollision = true;
				bestGroundY = my_max(bestGroundY, blockAABB.max.y);
			}
		}

		// ★落下中だけ「床」として確定して速度を止める
		if (anyCollision && data_.translate.velocity.y <= 0.0f)
		{
			data_.translate.value.y = bestGroundY + halfY;
			playerAabb.min.y = data_.translate.value.y - halfY;
			playerAabb.max.y = data_.translate.value.y + halfY;

			data_.translate.velocity.y = 0.0f;
			data_.translate.acceleration.y = 0.0f;
		}
		else
		{
			// ★床ではない（または上昇中）なら重力をかける
			data_.translate.acceleration.y = GRAVITY;
		}
	}
#pragma endregion

#pragma region 上方向
	{
		Vector3 upCorners[4] = {
			Vector3(playerAabb.min.x, playerAabb.max.y + probe, playerAabb.min.z),
			Vector3(playerAabb.max.x, playerAabb.max.y + probe, playerAabb.min.z),
			Vector3(playerAabb.min.x, playerAabb.max.y + probe, playerAabb.max.z),
			Vector3(playerAabb.max.x, playerAabb.max.y + probe, playerAabb.max.z)
		};

		bool anyCollision = false;
		float bestCeilY = 999999.0f;

		for (int i = 0; i < 4; ++i)
		{
			AABB blockAABB = mapManager_->GetAABB(upCorners[i]);

			if (IsCollision(blockAABB, playerAabb) && mapManager_->GetIsActive(upCorners[i]))
			{
				anyCollision = true;
				bestCeilY = my_min(bestCeilY, blockAABB.min.y);
			}
		}

		if (anyCollision)
		{
			// playerAabb.max.y == bestCeilY になるように中心を設定
			data_.translate.value.y = bestCeilY - halfY;
			playerAabb.min.y = data_.translate.value.y - halfY;
			playerAabb.max.y = data_.translate.value.y + halfY;

			// 上向き速度を止める
			if (data_.translate.velocity.y > 0.0f) data_.translate.velocity.y = 0.0f;
		}
	}
#pragma endregion

#pragma region +X方向（右壁）
	{
		Vector3 corners[4] = {
			Vector3(playerAabb.max.x + probe, playerAabb.min.y, playerAabb.min.z),
			Vector3(playerAabb.max.x + probe, playerAabb.min.y, playerAabb.max.z),
			Vector3(playerAabb.max.x + probe, playerAabb.max.y, playerAabb.min.z),
			Vector3(playerAabb.max.x + probe, playerAabb.max.y, playerAabb.max.z),
		};

		bool anyCollision = false;
		float bestWallX = 999999.0f;

		for (int i = 0; i < 4; ++i)
		{
			AABB blockAABB = mapManager_->GetAABB(corners[i]);

			if (IsCollision(blockAABB, playerAabb) && mapManager_->GetIsActive(corners[i]))
			{
				anyCollision = true;
				bestWallX = my_min(bestWallX, blockAABB.min.x);
			}
		}

		if (anyCollision)
		{
			data_.translate.value.x = bestWallX - halfX;
			playerAabb.min.x = data_.translate.value.x - halfX;
			playerAabb.max.x = data_.translate.value.x + halfX;

			if (data_.translate.velocity.x > 0.0f) data_.translate.velocity.x = 0.0f;
		}
	}
#pragma endregion

#pragma region -X方向（左壁）
	{
		Vector3 corners[4] = {
			Vector3(playerAabb.min.x - probe, playerAabb.min.y, playerAabb.min.z),
			Vector3(playerAabb.min.x - probe, playerAabb.min.y, playerAabb.max.z),
			Vector3(playerAabb.min.x - probe, playerAabb.max.y, playerAabb.min.z),
			Vector3(playerAabb.min.x - probe, playerAabb.max.y, playerAabb.max.z),
		};

		bool anyCollision = false;
		float bestWallX = -999999.0f;

		for (int i = 0; i < 4; ++i)
		{
			AABB blockAABB = mapManager_->GetAABB(corners[i]);

			if (IsCollision(blockAABB, playerAabb) && mapManager_->GetIsActive(corners[i]))
			{
				anyCollision = true;
				bestWallX = my_max(bestWallX, blockAABB.max.x);
			}
		}

		if (anyCollision)
		{
			data_.translate.value.x = bestWallX + halfX;
			playerAabb.min.x = data_.translate.value.x - halfX;
			playerAabb.max.x = data_.translate.value.x + halfX;

			if (data_.translate.velocity.x < 0.0f) data_.translate.velocity.x = 0.0f;
		}
	}
#pragma endregion

#pragma region +Z方向（前壁）
	{
		Vector3 corners[4] = {
			Vector3(playerAabb.min.x, playerAabb.min.y, playerAabb.max.z + probe),
			Vector3(playerAabb.max.x, playerAabb.min.y, playerAabb.max.z + probe),
			Vector3(playerAabb.min.x, playerAabb.max.y, playerAabb.max.z + probe),
			Vector3(playerAabb.max.x, playerAabb.max.y, playerAabb.max.z + probe),
		};

		bool anyCollision = false;
		float bestWallZ = 999999.0f;

		for (int i = 0; i < 4; ++i)
		{
			AABB blockAABB = mapManager_->GetAABB(corners[i]);

			if (IsCollision(blockAABB, playerAabb) && mapManager_->GetIsActive(corners[i]))
			{
				anyCollision = true;
				bestWallZ = my_min(bestWallZ, blockAABB.min.z);
			}
		}

		if (anyCollision)
		{
			data_.translate.value.z = bestWallZ - halfZ;
			playerAabb.min.z = data_.translate.value.z - halfZ;
			playerAabb.max.z = data_.translate.value.z + halfZ;

			if (data_.translate.velocity.z > 0.0f) data_.translate.velocity.z = 0.0f;
		}
	}
#pragma endregion

#pragma region -Z方向（後壁）
	{
		Vector3 corners[4] = {
			Vector3(playerAabb.min.x, playerAabb.min.y, playerAabb.min.z - probe),
			Vector3(playerAabb.max.x, playerAabb.min.y, playerAabb.min.z - probe),
			Vector3(playerAabb.min.x, playerAabb.max.y, playerAabb.min.z - probe),
			Vector3(playerAabb.max.x, playerAabb.max.y, playerAabb.min.z - probe),
		};

		bool anyCollision = false;
		float bestWallZ = -999999.0f;

		for (int i = 0; i < 4; ++i)
		{
			AABB blockAABB = mapManager_->GetAABB(corners[i]);

			if (IsCollision(blockAABB, playerAabb) && mapManager_->GetIsActive(corners[i]))
			{
				anyCollision = true;
				bestWallZ = my_max(bestWallZ, blockAABB.max.z);
			}
		}

		if (anyCollision)
		{
			data_.translate.value.z = bestWallZ + halfZ;
			playerAabb.min.z = data_.translate.value.z - halfZ;
			playerAabb.max.z = data_.translate.value.z + halfZ;

			if (data_.translate.velocity.z < 0.0f) data_.translate.velocity.z = 0.0f;
		}
	}
#pragma endregion
}

void BaseCharactor::StepPhysicsAndCollision()
{
	if (!mapManager_) return;

	// 1) 速度だけ更新（加速度→速度）
	data_.UpdateVelocitiesPhysics();

	// 2) 今フレームの移動量 delta
	float deltaTime = Game::Time::GetDeltaTime();
	deltaTime *= 60.0f;
	const Vector3 delta = data_.translate.velocity * deltaTime;

	// 3) 移動前AABB更新（Sweep入力用）
	data_.UpdateLocalMatrix();
	data_.UpdateWorldMatrix();
	data_.UpdateAABB();

	// 4) Sweep
	Vector3 correctedDelta = delta;
	mapManager_->SweepAABB(data_.aabbs[0], delta, correctedDelta);

	// 5) 移動適用
	data_.ApplyTranslationDelta(correctedDelta);

	// 6) 速度停止（当たった軸）
	const bool hitX = std::abs(correctedDelta.x - delta.x) > eps;
	const bool hitY = std::abs(correctedDelta.y - delta.y) > eps;
	const bool hitZ = std::abs(correctedDelta.z - delta.z) > eps;

	if (hitX) data_.translate.velocity.x = 0.0f;
	if (hitY) data_.translate.velocity.y = 0.0f;
	if (hitZ) data_.translate.velocity.z = 0.0f;

	// 7) AABB更新（接地判定用）
	data_.UpdateLocalMatrix();
	data_.UpdateWorldMatrix();
	data_.UpdateAABB();

	// 接地判定：①落下中にYが潰れた ②静止中でも少し下へSweepすると潰れる
	auto IsGroundedBySweep = [&](const AABB& aabb) -> bool
		{
			const Vector3 downDelta{ 0.0f, -0.05f, 0.0f };
			Vector3 corrected = downDelta;
			mapManager_->SweepAABB(aabb, downDelta, corrected);
			return std::abs(corrected.y - downDelta.y) > eps;
		};

	const bool landedThisFrame = (delta.y < 0.0f && hitY);
	isGrounded_ = landedThisFrame || IsGroundedBySweep(data_.aabbs[0]);

	// 重力
	data_.translate.acceleration.y = isGrounded_ ? 0.0f : GRAVITY;
}

void BaseCharactor::Jump()
{
	// 接地していないならジャンプ不可
	//if (!isGrounded_) return;

	data_.translate.velocity.y = jumpPower_;
	data_.translate.acceleration.y = GRAVITY; // ジャンプ後は重力を戻す
	isGrounded_ = false;
}

void BaseCharactor::Move(const Vector3& direction, float speed)
{
	data_.translate.velocity.x = direction.x * speed;
	data_.translate.velocity.z = direction.z * speed;
}


void BaseCharactor::BreakTargetBlock()
{
	if (!targetBlock_.has_value()) return;

	lookAtBlock lab = targetBlock_.value();
	Block* block = lab.block;
	if (!block) return;

	block->durability_->DecreaseDurability(breakPower_);

	// 破壊されていたら非アクティブ化
	if (block->durability_->GetIsDestroy())
	{
		mapManager_->DestroyBlockAt(lab.chunkIndex, lab.localIndex);
	}
}

void BaseCharactor::SetNewBlock(BlockID id)
{
	if (!targetBlock_.has_value()) return;

	lookAtBlock lab = targetBlock_.value();

	// 当たった面情報が無いなら置けない
	if (lab.face == AABBFace::NONE) return;

	// MapManagerから設置処理を呼び出す
	mapManager_->SetBlockAt(lab, id);
}
