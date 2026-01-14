#pragma once
#include <vector>
#include "definition/definition.h"
#include "Physics/IWorldCollider.h"
#include "Physics/IPhysicsBody.h"
#include "Physics/MapWorldCollider.h"

class PhysicsSystem
{
public:
	// WorldColliderの設定
    void SetIWorldCollider(MapWorldCollider* world) { world_ = world; }

    /// IPhysicsBodyの物理演算有効化
    void RegisterDynamic(IPhysicsBody* model) { dynamics_.push_back(model); }

    /// IPhysicsBodyの物理演算無効化
    void UnregisterDynamic(IPhysicsBody* model);

    /// 登録されている全てのIPhysicsBodyの物理演算無効化
    void ClearDynamics() { dynamics_.clear(); }

    void Step();

private:
    const MapWorldCollider* world_ = nullptr;
    std::vector<IPhysicsBody*> dynamics_;
};

 