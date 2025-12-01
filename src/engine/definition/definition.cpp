#include "definition.h"
#include<cmath>
#include<cstdint>
#include<numbers>

#pragma region Vector2

float Vector2::Length() const
{
    return std::sqrt(x * x + y * y);
}

float Vector2::LengthSq() const
{
    return x * x + y * y;
}

Vector2& Vector2::Normalize()
{
    float len = Length();
    const float EPSILON = 0.00001f;
    if (std::abs(len) > EPSILON)
    {
        x /= len;
        y /= len;
    }
    return *this;
}

Vector2 Vector2::Normalized() const
{
    Vector2 result = *this;
    result.Normalize();
    return result;
}

float Vector2::Dot(const Vector2& rhs) const
{
    return x * rhs.x + y * rhs.y;
}

#pragma endregion

#pragma region Vector3

float Vector3::Length() const
{
    return std::sqrt(x * x + y * y + z * z);
}

float Vector3::LengthSq() const
{
    return x * x + y * y + z * z;
}

Vector3& Vector3::Normalize()
{
    float len = Length();
    if (std::abs(len) > 0.00001f)
    {
        x /= len;
        y /= len;
        z /= len;
    }
    return *this;
}

Vector3 Vector3::Normalized() const
{
    Vector3 result = *this;
    result.Normalize();
    return result;
}

float Vector3::Dot(const Vector3& rhs) const
{
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vector3 Vector3::Cross(const Vector3& rhs) const
{
    return Vector3(
        y * rhs.z - z * rhs.y,
        z * rhs.x - x * rhs.z,
        x * rhs.y - y * rhs.x
    );
}

Vector3 Vector3::Reflect(const Vector3& input, const Vector3& normal)
{
    Vector3 result;

    result = input - ((normal * (input.Dot(normal))) * 2);

    return result;
}

#pragma endregion

#pragma region Vector4

float Vector4::Length() const
{
    return std::sqrt(x * x + y * y + z * z + w * w);
}

float Vector4::LengthSq() const
{
    return x * x + y * y + z * z + w * w;
}

Vector4& Vector4::Normalize()
{
    float len = Length();
    const float EPSILON = 0.00001f;
    if (std::abs(len) > EPSILON)
    {
        x /= len;
        y /= len;
        z /= len;
        w /= len;
    }
    return *this;
}

Vector4 Vector4::Normalized() const
{
    Vector4 result = *this;
    result.Normalize();
    return result;
}

float Vector4::Dot(const Vector4& rhs) const
{
    return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

#pragma endregion

#pragma region Matrix4x4

Matrix4x4 Matrix4x4::Inverse() const
{
    Matrix4x4 Return{};

    float det = (m[0][0] * m[1][1] * m[2][2] * m[3][3]) + (m[0][0] * m[1][2] * m[2][3] * m[3][1]) + (m[0][0] * m[1][3] * m[2][1] * m[3][2])
        - (m[0][0] * m[1][3] * m[2][2] * m[3][1]) - (m[0][0] * m[1][2] * m[2][1] * m[3][3]) - (m[0][0] * m[1][1] * m[2][3] * m[3][2])
        - (m[0][1] * m[1][0] * m[2][2] * m[3][3]) - (m[0][2] * m[1][0] * m[2][3] * m[3][1]) - (m[0][3] * m[1][0] * m[2][1] * m[3][2])
        + (m[0][3] * m[1][0] * m[2][2] * m[3][1]) + (m[0][2] * m[1][0] * m[2][1] * m[3][3]) + (m[0][1] * m[1][0] * m[2][3] * m[3][2])
        + (m[0][1] * m[1][2] * m[2][0] * m[3][3]) + (m[0][2] * m[1][3] * m[2][0] * m[3][1]) + (m[0][3] * m[1][1] * m[2][0] * m[3][2])
        - (m[0][3] * m[1][2] * m[2][0] * m[3][1]) - (m[0][2] * m[1][1] * m[2][0] * m[3][3]) - (m[0][1] * m[1][3] * m[2][0] * m[3][2])
        - (m[0][1] * m[1][2] * m[2][3] * m[3][0]) - (m[0][2] * m[1][3] * m[2][1] * m[3][0]) - (m[0][3] * m[1][1] * m[2][2] * m[3][0])
        + (m[0][3] * m[1][2] * m[2][1] * m[3][0]) + (m[0][2] * m[1][1] * m[2][3] * m[3][0]) + (m[0][1] * m[1][3] * m[2][2] * m[3][0]);

    // Protect against near-zero determinant (use absolute value)
    if (std::fabs(det) < eps) return Return;

    const float invDet = 1.0f / det;

    Return.m[0][0] = invDet * ((m[1][1] * m[2][2] * m[3][3]) + (m[1][2] * m[2][3] * m[3][1]) + (m[1][3] * m[2][1] * m[3][2]) - (m[1][3] * m[2][2] * m[3][1]) - (m[1][2] * m[2][1] * m[3][3]) - (m[1][1] * m[2][3] * m[3][2]));
    Return.m[0][1] = invDet * ((m[0][3] * m[2][2] * m[3][1]) + (m[0][2] * m[2][1] * m[3][3]) + (m[0][1] * m[2][3] * m[3][2]) - (m[0][1] * m[2][2] * m[3][3]) - (m[0][2] * m[2][3] * m[3][1]) - (m[0][3] * m[2][1] * m[3][2]));
    Return.m[0][2] = invDet * ((m[0][1] * m[1][2] * m[3][3]) + (m[0][2] * m[1][3] * m[3][1]) + (m[0][3] * m[1][1] * m[3][2]) - (m[0][3] * m[1][2] * m[3][1]) - (m[0][2] * m[1][1] * m[3][3]) - (m[0][1] * m[1][3] * m[3][2]));
    Return.m[0][3] = invDet * ((m[0][3] * m[1][2] * m[2][1]) + (m[0][2] * m[1][1] * m[2][3]) + (m[0][1] * m[1][3] * m[2][2]) - (m[0][1] * m[1][2] * m[2][3]) - (m[0][2] * m[1][3] * m[2][1]) - (m[0][3] * m[1][1] * m[2][2]));

    Return.m[1][0] = invDet * ((m[1][3] * m[2][2] * m[3][0]) + (m[1][2] * m[2][0] * m[3][3]) + (m[1][0] * m[2][3] * m[3][2]) - (m[1][0] * m[2][2] * m[3][3]) - (m[1][2] * m[2][3] * m[3][0]) - (m[1][3] * m[2][0] * m[3][2]));
    Return.m[1][1] = invDet * ((m[0][0] * m[2][2] * m[3][3]) + (m[0][2] * m[2][3] * m[3][0]) + (m[0][3] * m[2][0] * m[3][2]) - (m[0][3] * m[2][2] * m[3][0]) - (m[0][2] * m[2][0] * m[3][3]) - (m[0][0] * m[2][3] * m[3][2]));
    Return.m[1][2] = invDet * ((m[0][3] * m[1][2] * m[3][0]) + (m[0][2] * m[1][0] * m[3][3]) + (m[0][0] * m[1][3] * m[3][2]) - (m[0][0] * m[1][2] * m[3][3]) - (m[0][2] * m[1][3] * m[3][0]) - (m[0][3] * m[1][0] * m[3][2]));
    Return.m[1][3] = invDet * ((m[0][0] * m[1][2] * m[2][3]) + (m[0][2] * m[1][3] * m[2][0]) + (m[0][3] * m[1][0] * m[2][2]) - (m[0][3] * m[1][2] * m[2][0]) - (m[0][2] * m[1][0] * m[2][3]) - (m[0][0] * m[1][3] * m[2][2]));

    Return.m[2][0] = invDet * ((m[1][0] * m[2][1] * m[3][3]) + (m[1][1] * m[2][3] * m[3][0]) + (m[1][3] * m[2][0] * m[3][1]) - (m[1][3] * m[2][1] * m[3][0]) - (m[1][1] * m[2][0] * m[3][3]) - (m[1][0] * m[2][3] * m[3][1]));
    Return.m[2][1] = invDet * ((m[0][3] * m[2][1] * m[3][0]) + (m[0][1] * m[2][0] * m[3][3]) + (m[0][0] * m[2][3] * m[3][1]) - (m[0][0] * m[2][1] * m[3][3]) - (m[0][1] * m[2][3] * m[3][0]) - (m[0][3] * m[2][0] * m[3][1]));
    Return.m[2][2] = invDet * ((m[0][0] * m[1][1] * m[3][3]) + (m[0][1] * m[1][3] * m[3][0]) + (m[0][3] * m[1][0] * m[3][1]) - (m[0][3] * m[1][1] * m[3][0]) - (m[0][1] * m[1][0] * m[3][3]) - (m[0][0] * m[1][3] * m[3][1]));
    Return.m[2][3] = invDet * ((m[0][3] * m[1][1] * m[2][0]) + (m[0][1] * m[1][0] * m[2][3]) + (m[0][0] * m[1][3] * m[2][1]) - (m[0][0] * m[1][1] * m[2][3]) - (m[0][1] * m[1][3] * m[2][0]) - (m[0][3] * m[1][0] * m[2][1]));

    Return.m[3][0] = invDet * ((m[1][2] * m[2][1] * m[3][0]) + (m[1][1] * m[2][0] * m[3][2]) + (m[1][0] * m[2][2] * m[3][1]) - (m[1][0] * m[2][1] * m[3][2]) - (m[1][1] * m[2][2] * m[3][0]) - (m[1][2] * m[2][0] * m[3][1]));
    Return.m[3][1] = invDet * ((m[0][0] * m[2][1] * m[3][2]) + (m[0][1] * m[2][2] * m[3][0]) + (m[0][2] * m[2][0] * m[3][1]) - (m[0][2] * m[2][1] * m[3][0]) - (m[0][1] * m[2][0] * m[3][2]) - (m[0][0] * m[2][2] * m[3][1]));
    Return.m[3][2] = invDet * ((m[0][2] * m[1][1] * m[3][0]) + (m[0][1] * m[1][0] * m[3][2]) + (m[0][0] * m[1][2] * m[3][1]) - (m[0][0] * m[1][1] * m[3][2]) - (m[0][1] * m[1][2] * m[3][0]) - (m[0][2] * m[1][0] * m[3][1]));
    Return.m[3][3] = invDet * ((m[0][0] * m[1][1] * m[2][2]) + (m[0][1] * m[1][2] * m[2][0]) + (m[0][2] * m[1][0] * m[2][1]) - (m[0][2] * m[1][1] * m[2][0]) - (m[0][1] * m[1][0] * m[2][2]) - (m[0][0] * m[1][2] * m[2][1]));

    return Return;
}

Matrix4x4 Matrix4x4::Transpose() const
{
    Matrix4x4 Return{};

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            Return.m[i][j] = m[j][i];
        }
    }

    return Return;
}

Matrix4x4 Matrix4x4::MakeIdentity4x4()
{
    Matrix4x4 Return{};

    for (int i = 0; i < 4; ++i)
    {
        Return.m[i][i] = 1.0f;
    }

    return Return;
}

Matrix4x4 Matrix4x4::MakeTranslateMatrix(const Vector3& translate)
{
    Matrix4x4 result = MakeIdentity4x4();
    result.m[3][0] = translate.x;
    result.m[3][1] = translate.y;
    result.m[3][2] = translate.z;
    return result;
}

Matrix4x4 Matrix4x4::MakeScaleMatrix(const Vector3& scale)
{
    Matrix4x4 result = MakeIdentity4x4();
    result.m[0][0] = scale.x;
    result.m[1][1] = scale.y;
    result.m[2][2] = scale.z;

    return result;
}

Matrix4x4 Matrix4x4::MakeRotateXMatrix(float radian)
{
    Matrix4x4 Return = MakeIdentity4x4();

    Return.m[1][1] = std::cos(radian);
    Return.m[2][1] = -std::sin(radian);
    Return.m[1][2] = std::sin(radian);
    Return.m[2][2] = std::cos(radian);

    return Return;
}

Matrix4x4 Matrix4x4::MakeRotateYMatrix(float radian)
{
    Matrix4x4 Return = MakeIdentity4x4();

    Return.m[0][0] = std::cos(radian);
    Return.m[2][0] = std::sin(radian);
    Return.m[0][2] = -std::sin(radian);
    Return.m[2][2] = std::cos(radian);

    return Return;
}

Matrix4x4 Matrix4x4::MakeRotateZMatrix(float radian)
{
    Matrix4x4 Return = MakeIdentity4x4();

    Return.m[0][0] = std::cos(radian);
    Return.m[1][0] = -std::sin(radian);
    Return.m[0][1] = std::sin(radian);
    Return.m[1][1] = std::cos(radian);

    return Return;
}

Matrix4x4 Matrix4x4::MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate)
{
    Matrix4x4 Return{};

    Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
    Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
    Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
    Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
    Matrix4x4 rotateXYZMatrix = rotateZMatrix * rotateXMatrix * rotateYMatrix;
    Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
    Matrix4x4 resultMatrix = scaleMatrix * rotateXYZMatrix * translateMatrix;

    return resultMatrix;
}

Matrix4x4 Matrix4x4::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
    Matrix4x4 Return{};

    float tanHalfFovY = std::tan(fovY / 2.0f);
    Return.m[0][0] = 1.0f / (aspectRatio * tanHalfFovY);
    Return.m[1][1] = 1.0f / tanHalfFovY;
    Return.m[2][2] = farClip / (farClip - nearClip);
    Return.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
    Return.m[2][3] = 1.0f;

    return Return;
}

Matrix4x4 Matrix4x4::MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
{
    Matrix4x4 Return{};

    Return.m[0][0] = 2.0f / (right - left);
    Return.m[0][1] = 0.0f;
    Return.m[0][2] = 0.0f;
    Return.m[0][3] = 0.0f;

    Return.m[1][0] = 0.0f;
    Return.m[1][1] = 2.0f / (top - bottom);
    Return.m[1][2] = 0.0f;
    Return.m[1][3] = 0.0f;

    Return.m[2][0] = 0.0f;
    Return.m[2][1] = 0.0f;
    Return.m[2][2] = 1.0f / (farClip - nearClip);
    Return.m[2][3] = 0.0f;

    Return.m[3][0] = (left + right) / (left - right);
    Return.m[3][1] = (top + bottom) / (bottom - top);
    Return.m[3][2] = (nearClip) / (nearClip - farClip);
    Return.m[3][3] = 1.0f;

    return Return;
}

Matrix4x4 Matrix4x4::MakeViewPortMatrix(float left, float top, float width, float height, float minD, float maxD)
{
    Matrix4x4 Return{};
    //// 最小深度値
    //float minD = 0;
    //// 最大深度値
    //float maxD = 1;


    Return.m[0][0] = width / 2.0f;
    Return.m[0][1] = 0.0f;
    Return.m[0][2] = 0.0f;
    Return.m[0][3] = 0.0f;

    Return.m[1][0] = 0.0f;
    Return.m[1][1] = -height / 2.0f;
    Return.m[1][2] = 0.0f;
    Return.m[1][3] = 0.0f;

    Return.m[2][0] = 0.0f;
    Return.m[2][1] = 0.0f;
    Return.m[2][2] = maxD - minD;
    Return.m[2][3] = 0.0f;

    Return.m[3][0] = left + (width / 2.0f);
    Return.m[3][1] = top + (height / 2.0f);
    Return.m[3][2] = minD;
    Return.m[3][3] = 1.0f;


    return Return;
}


#pragma endregion

#pragma region AABB

Vector3 AABB::center()const
{
    return Vector3{
        (min.x + max.x) / 2.0f,
        (min.y + max.y) / 2.0f,
        (min.z + max.z) / 2.0f,
    };
}

void AABB::Fix()
{
    AABB aabb = *this;
    this->min.x = my_min(aabb.min.x, aabb.max.x);
    this->max.x = my_max(aabb.min.x, aabb.max.x);
    this->min.y = my_min(aabb.min.y, aabb.max.y);
    this->max.y = my_max(aabb.min.y, aabb.max.y);
    this->min.z = my_min(aabb.min.z, aabb.max.z);
    this->max.z = my_max(aabb.min.z, aabb.max.z);
};

Vector3 AABB::GetCollisionDepth(const AABB& other)const
{
    Vector3 depth;
    depth.x = my_min(this->max.x, other.max.x) - my_max(this->min.x, other.min.x);
    depth.y = my_min(this->max.y, other.max.y) - my_max(this->min.y, other.min.y);
    depth.z = my_min(this->max.z, other.max.z) - my_max(this->min.z, other.min.z);
    if (depth.x <= 0.0f || depth.y <= 0.0f || depth.z <= 0.0f)
    {
        return { 0.0f, 0.0f, 0.0f };
    }
    return depth;
}

#pragma endregion

std::string BlockIDToString(BlockID id)
{
    switch (id)
    {
    case BlockID::Air:
        return "Air";
        break;
    case BlockID::Stone:
        return "Stone";
        break;
    case BlockID::Glass:
        return "Glass";
        break;
    case BlockID::Dirt:
        return "Dirt";
        break;
    case BlockID::Lawn:
        return "Lawn";
        break;
    case BlockID::Wood:
        return "Wood";
        break;
    case BlockID::Leaf:
        return "Leaf";
        break;
    case BlockID::MAX:
    default:
        return "Unknown";
        break;
    }
}
