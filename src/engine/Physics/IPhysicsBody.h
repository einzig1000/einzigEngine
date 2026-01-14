#pragma once
#include "definition/definition.h"
#include <span>
#include <vector>

/// <summary>
/// 物理演算対象インターフェース
/// </summary>

class IPhysicsBody
{
public:
    virtual ~IPhysicsBody() = default;

    // 衝突形状（AABB群）
    virtual std::span<const AABB> GetAABBs() const = 0;

    // Sweep用：移動前状態でAABBを最新化
    virtual void SyncAABBForSweep() = 0;

    // 速度・加速度の積分（acceleration -> velocity）
    virtual void IntegrateVelocity(float dt) = 0;

    // 速度の取得/設定（衝突後に軸速度を0にするため）
    virtual Vector3 GetVelocity() const = 0;
    virtual void SetVelocity(const Vector3& v) = 0;

    // 位置に移動量を適用
    virtual void ApplyTranslationDelta(const Vector3& delta) = 0;

    // 移動後の最終同期（AABB更新など）
    virtual void SyncAfterMove() = 0;
};