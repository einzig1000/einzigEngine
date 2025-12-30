#pragma once
#include <vector>
#include "definition/definition.h"
#include "DrawSystem/RenderData/RenderData.h"
#include "Physics/IWorldCollider.h"

/// <summary>
/// WorldColliderが提供する当たり判定機能を用いて、
/// RenderData_Modelの物理演算を行うシステム
/// </summary>

class PhysicsSystem
{
public:
	// WorldColliderの設定
    void SetIWorldCollider(IWorldCollider* world) { world_ = world; }

    /// RenderData_Modelの物理演算有効化
    void RegisterDynamic(RenderData_Model* model) { dynamics_.push_back(model); }

    /// RenderData_Modelの物理演算無効化
    void UnregisterDynamic(RenderData_Model* model);

    /// 登録されている全てのRenderData_Modelの物理演算無効化
    void ClearDynamics() { dynamics_.clear(); }

    void Step();

private:
    const IWorldCollider* world_ = nullptr;
    std::vector<RenderData_Model*> dynamics_;
};

 