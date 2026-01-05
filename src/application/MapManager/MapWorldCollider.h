#pragma once
#include "Physics/IWorldCollider.h"

class MapManager;

class MapWorldCollider final : public IWorldCollider
{
public:
    explicit MapWorldCollider(const MapManager* map) : map_(map) {}

    bool SweepAABB(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const override;
    bool IsSolidAt(const Vector3& position) const override;

private:
    const MapManager* map_ = nullptr;
};