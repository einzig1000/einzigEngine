#include "definition.h"
#include<cmath>
#include<cstdint>
#include<numbers>

quaternion quaternion::MakeIdentityQuaternion()
{
	return quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

quaternion quaternion::MakeConjugateQuaternion(const quaternion& q)
{
	return quaternion(-q.x, -q.y, -q.z, q.w);
}

float quaternion::Norm(const quaternion& q)
{
	return std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

quaternion quaternion::Normalize(const quaternion& q)
{
    float norm = Norm(q);
    if (std::abs(norm) > eps)
    {
        return quaternion(
            q.x / std::sqrt(norm),
            q.y / std::sqrt(norm),
            q.z / std::sqrt(norm),
            q.w / std::sqrt(norm)
        );
    }
	return quaternion();
}

quaternion quaternion::Inverse(const quaternion& q)
{
    float norm = Norm(q);
    if (std::abs(norm) > eps)
    {
        quaternion conjugate = MakeConjugateQuaternion(q);
        return quaternion(
            conjugate.x / norm,
            conjugate.y / norm,
            conjugate.z / norm,
            conjugate.w / norm
        );
    }
	return quaternion();
}
