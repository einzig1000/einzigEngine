#include "PhysicsSystem.h"
#include "Game.h"

void PhysicsSystem::UnregisterDynamic(RenderData_Model* model)
{
	dynamics_.erase(std::remove(dynamics_.begin(), dynamics_.end(), model), dynamics_.end());
}

void PhysicsSystem::Step()
{
    if (!world_) return;

	float dt = Game::Time::GetDeltaTime() * 60.0f;

    for (RenderData_Model* m : dynamics_)
    {
        if (!m) continue;
        if (m->aabbs.empty()) continue;

		// 1) 速度に加速度を加算
        m->UpdateVelocitiesPhysics();

		// 2) 理想移動量計算
        const Vector3 delta = m->translate.velocity * dt;

		// 3) Sweep入力用にAABB更新(移動前座標)
        m->UpdateLocalMatrix();
        m->UpdateWorldMatrix();
        m->UpdateAABB();

		// 4) Sweep(correctedは固体にぶつかるまでの移動量)
        Vector3 corrected = delta;
        world_->SweepAABB(m->aabbs[0], delta, corrected);

        // 5) 位置反映
        m->ApplyTranslationDelta(corrected);

        // 6) 当たった軸の速度を0に
        const bool hitX = std::abs(corrected.x - delta.x) > eps;
        const bool hitY = std::abs(corrected.y - delta.y) > eps;
        const bool hitZ = std::abs(corrected.z - delta.z) > eps;

        if (hitX) m->translate.velocity.x = 0.0f;
        if (hitZ) m->translate.velocity.z = 0.0f;
        // Yは「落下で床に当たった」場合だけ止める（壁張り付き防止）
        if (hitY)
        {
            if (delta.y < 0.0f && corrected.y > delta.y)
            {
                // 下向き移動が縮んだ（床に当たって止まった）
                m->translate.velocity.y = 0.0f;
            }
            else if (delta.y > 0.0f && corrected.y < delta.y)
            {
                // 上向き移動が縮んだ（天井に当たって止まった）
                m->translate.velocity.y = 0.0f;
            }
            // それ以外（壁衝突の副作用など）ではY速度は触らない
        }

        // 7) 最終AABB更新
        m->UpdateLocalMatrix();
        m->UpdateWorldMatrix();
        m->UpdateAABB();
        m->UpdateInPicture();
        m->SavePreTransforms();
    }
}