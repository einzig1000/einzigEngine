#pragma once
#include "definition/definition.h"

class IWorldCollider
{
public:
	virtual ~IWorldCollider() = default;

    // AABB sweep: delta を outCorrectedDelta に補正し、何かヒットしたら true
    virtual bool SweepAABB(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const = 0;

	// その座標に固体(当たり判定があるか)を返す
    virtual bool IsSolidAt(const Vector3& position) const = 0;
};

