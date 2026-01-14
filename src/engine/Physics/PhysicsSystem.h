#pragma once
#include <vector>
#include "definition/definition.h"
#include "memory.h"
class IWorldCollider;
class IPhysicsBody;
class MapWorldCollider;

class PhysicsSystem
{
public:
    PhysicsSystem();

	// コライダーの追加
	void AddWorldCollider(IWorldCollider* collider);

    /// IPhysicsBodyの物理演算有効化
    void RegisterDynamic(IPhysicsBody* model) { dynamics_.push_back(model); }

    /// IPhysicsBodyの物理演算無効化
    void UnregisterDynamic(IPhysicsBody* model);

    /// 登録されている全てのIPhysicsBodyの物理演算無効化
    void ClearDynamics() { dynamics_.clear(); }

    void Step();

private:
	std::unique_ptr<MapWorldCollider> world_;
    std::vector<IPhysicsBody*> dynamics_;
};

 