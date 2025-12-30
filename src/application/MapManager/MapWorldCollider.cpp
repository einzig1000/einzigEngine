#include "MapWorldCollider.h"
#include "MapManager/MapManager.h"

bool MapWorldCollider::SweepAABB(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const
{
    if (!map_) { outCorrectedDelta = delta; return false; }
    return map_->SweepAABB(aabb, delta, outCorrectedDelta);
}

bool MapWorldCollider::IsSolidAt(const Vector3& position) const
{
    if (!map_) return false;
    return map_->isSolidAt(position);
}