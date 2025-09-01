#include "definition.h"
#include "Utilities/Quaternion.h"

Matrix4x4 Matrix4x4::MakeFromQuaternion(const Quaternion& q)
{
    float x2 = q.x * q.x * 2.0f;
    float y2 = q.y * q.y * 2.0f;
    float z2 = q.z * q.z * 2.0f;
    float xy = q.x * q.y * 2.0f;
    float xz = q.x * q.z * 2.0f;
    float yz = q.y * q.z * 2.0f;
    float wx = q.w * q.x * 2.0f;
    float wy = q.w * q.y * 2.0f;
    float wz = q.w * q.z * 2.0f;

    Matrix4x4 result;
    result.m[0][0] = 1.0f - y2 - z2;
    result.m[0][1] = xy + wz;
    result.m[0][2] = xz - wy;
    result.m[0][3] = 0.0f;

    result.m[1][0] = xy - wz;
    result.m[1][1] = 1.0f - x2 - z2;
    result.m[1][2] = yz + wx;
    result.m[1][3] = 0.0f;

    result.m[2][0] = xz + wy;
    result.m[2][1] = yz - wx;
    result.m[2][2] = 1.0f - x2 - y2;
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}