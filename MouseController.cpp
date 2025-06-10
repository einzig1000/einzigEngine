#include "MouseController.h"
#include "functions.h"

void MouseController::SetMouseRay(const uint32_t width, const uint32_t height, const Matrix4x4 viewProjectionMatrix)
{
    // 左下が０、右上が１とした時のマウスポジション
    float ndcX = (position_.x / width) * 2.0f - 1.0f;
    float ndcY = 1.0f - (position_.y / height) * 2.0f; // Yは上下反転

    // クリップ空間でZ=0(near)とZ=1(far)の2点を作る
    Vector4 nearPoint = { ndcX, ndcY, 0.0f, 1.0f };
    Vector4 farPoint = { ndcX, ndcY, 1.0f, 1.0f };

    // 逆射影行列
    Matrix4x4 invViewProj = viewProjectionMatrix.Inverse();

    // ワールド空間に変換
    Vector4 nearWorld = Transform(nearPoint, invViewProj);
    Vector4 farWorld = Transform(farPoint, invViewProj);

    // マウスレイの始点・方向
    ray_.origin = { nearWorld.x / nearWorld.w, nearWorld.y / nearWorld.w, nearWorld.z / nearWorld.w };
    ray_.diff = Vector3{
    (farWorld.x / farWorld.w) - ray_.origin.x,
    (farWorld.y / farWorld.w) - ray_.origin.y,
    (farWorld.z / farWorld.w) - ray_.origin.z
    }.Normalized();
}