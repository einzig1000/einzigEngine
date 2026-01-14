#include "Physics/MapWorldCollider.h"
#include "MapManager/MapManager.h"


MapWorldCollider::MapWorldCollider()
{}

void MapWorldCollider::AddCollider(IWorldCollider * collider)
{
	colliders_.push_back(collider);
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
