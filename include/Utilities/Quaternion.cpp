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

Quaternion Quaternion::MakeFromEulerAngles(const Vector3& eulerAngles)
{
    float halfX = eulerAngles.x * 0.5f;
    float halfY = eulerAngles.y * 0.5f;
    float halfZ = eulerAngles.z * 0.5f;

    float cx = cos(halfX);
    float sx = sin(halfX);
    float cy = cos(halfY);
    float sy = sin(halfY);
    float cz = cos(halfZ);
    float sz = sin(halfZ);

    Quaternion q;
    q.x = sx * cy * cz + cx * sy * sz;
    q.y = cx * sy * cz - sx * cy * sz;
    q.z = cx * cy * sz + sx * sy * cz;
    q.w = cx * cy * cz - sx * sy * sz;

    return q;
}

Quaternion Quaternion::operator*(const Quaternion& other) const
{
    return Quaternion(
        this->w * other.x + this->x * other.w + this->y * other.z - this->z * other.y,
        this->w * other.y - this->x * other.z + this->y * other.w + this->z * other.x,
        this->w * other.z + this->x * other.y - this->y * other.x + this->z * other.w,
        this->w * other.w - this->x * other.x - this->y * other.y - this->z * other.z
    );
}