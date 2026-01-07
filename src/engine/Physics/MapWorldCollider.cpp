#include "Physics/MapWorldCollider.h"
#include "MapManager/MapManager.h"


MapWorldCollider::MapWorldCollider()
{}

void MapWorldCollider::AddCollider(IWorldCollider * collider)
{
	colliders_.push_back(collider);
}

bool MapWorldCollider::SweepAABB(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const
{
	Vector3 correctedDelta = delta;
	bool hit = false;
	for (const auto& collider : colliders_)
	{
		Vector3 tempDelta;
		if (collider->SweepAABB(aabb, correctedDelta, tempDelta))
		{
			correctedDelta = tempDelta;
			hit = true;
		}
	}
	outCorrectedDelta = correctedDelta;
	return hit;
}

bool MapWorldCollider::isSolidAt(const Vector3& position) const
{
	for (const auto& collider : colliders_)
	{
		if (collider->IsSolidAt(position))
		{
			return true;
		}
	}
	return false;
}

//bool MapWorldCollider::SweepAABB(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const
//{
//    if (!map_) { outCorrectedDelta = delta; return false; }
//	// 0) 軸解決のみ
//    //return map_->SweepAABB(aabb, delta, outCorrectedDelta);
//	// 1) depenetration（先に「微妙な重なり」を毎フレ解消してから軸解決）
//    //return map_->SweepAABB_DepentrationFirst(aabb, delta, outCorrectedDelta);
//	// 2) 真正のSweep（TOI: time of impact）寄せ（「一番早く当たる面」を探して進める）
//	//return map_->SweepAABB_TOI(aabb, delta, outCorrectedDelta);
//	// 3) がっつりMTV方式（重なっているブロックとの“最小押し戻しベクトル”を計算して解消）
//	return map_->SweepAABB_MTV(aabb, delta, outCorrectedDelta);
//	// 4) サンプル点方式（面上の 3x3 サンプル点で押し戻し方向を決める）
//	//return map_->SweepAABB_SamplePoints(aabb, delta, outCorrectedDelta);
//}
//
//bool MapWorldCollider::IsSolidAt(const Vector3& position) const
//{
//    if (!map_) return false;
//    return map_->isSolidAt(position);
//}