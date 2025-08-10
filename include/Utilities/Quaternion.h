#pragma once
#include "definition/definition.h"

class Quaternion
{
public:
    float x, y, z, w;

    // デフォルトコンストラクタ
    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

    // コンストラクタ
    Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    // 2つのベクトル間の回転を表すクォータニオンを生成
    static Quaternion MakeFromToRotation(const Vector3& from, const Vector3& to);

    static Quaternion MakeFromEulerAngles(const Vector3& eulerAngles);



    Quaternion operator*(const Quaternion& other) const;
};