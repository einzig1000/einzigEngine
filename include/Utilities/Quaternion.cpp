#include "Utilities/Quaternion.h"

Quaternion Quaternion::MakeFromToRotation(const Vector3& from, const Vector3& to)
{
    // 2つのベクトルの正規化
    Vector3 v1 = from.Normalized();
    Vector3 v2 = to.Normalized();

    // 内積と外積の計算
    float dot = v1.Dot(v2);
    Vector3 cross = v1.Cross(v2);

    // 回転軸と角度からクォータニオンを生成
    if (dot >= 1.0f)
    {
        return Quaternion(); // 既に同じ方向を向いている場合、無回転
    }

    if (dot < -0.9999f) // 180度回転の場合
    {
        // 任意の軸（例：Y軸）を回転軸として使う
        return Quaternion(0, 1, 0, 0);
    }

    float s = sqrt((1.0f + dot) * 2.0f);
    float invs = 1.0f / s;

    return Quaternion(
        cross.x * invs,
        cross.y * invs,
        cross.z * invs,
        s * 0.5f
    );
}
