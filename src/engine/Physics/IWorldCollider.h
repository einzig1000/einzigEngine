#pragma once
#include "definition/definition.h"

class IWorldCollider
{
public:
	virtual ~IWorldCollider() = default;

	// その座標に固体(当たり判定があるか)を返す
    virtual bool IsSolidAt(const Vector3& position) const = 0;
};

