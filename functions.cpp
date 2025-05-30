

#include "functions.h"
#include "definition.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <format>
#include <algorithm>

//#include "externals/DirectXTex/DirectXTex.h"
#include <d3d12.h>
#include <wrl.h>

#define NOMINMAX
#include <windows.h>
#include <DbgHelp.h>
#include <strsafe.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "Dbghelp.lib")

template <typename T>
constexpr const T& my_min(const T& a, const T& b)
{
    return (a < b) ? a : b;
}

template <typename T>
constexpr const T& my_max(const T& a, const T& b)
{
    return (a > b) ? a : b;
}

template <typename T>
constexpr T my_sub(const T& a, const T& b)
{
    return a - b;
}

template <typename T>
constexpr T my_add(const T& a, const T& b)
{
    return a + b;
}


#pragma region Vector3

Vector3 Add(const Vector3& v1, const Vector3& v2)
{
    Vector3 Return{};

    Return.x = v1.x + v2.x;
    Return.y = v1.y + v2.y;
    Return.z = v1.z + v2.z;

    return Return;
}

Vector3 Sub(const Vector3& v1, const Vector3& v2)
{
    Vector3 Return{};

    Return.x = v1.x - v2.x;
    Return.y = v1.y - v2.y;
    Return.z = v1.z - v2.z;

    return Return;
}

Vector3 Mul(float scalar, const Vector3& v)
{
    Vector3 Return{};

    Return.x = scalar * v.x;
    Return.y = scalar * v.y;
    Return.z = scalar * v.z;

    return Return;
}

float DotProduct(const Vector3& v1, const Vector3& v2)
{
    float Return{};

    Return = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);

    return Return;
}

Vector3 CrossProduct(const Vector3& v1, const Vector3& v2)
{
    Vector3 result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}

float Length(const Vector3& v)
{
    float Return{};
    float a{};

    a = sqrtf((v.x * v.x) + (v.y * v.y));
    Return = sqrtf((a * a) + (v.z * v.z));

    return Return;
}

Vector3 CalculateNormal(const Vector4& v0, const Vector4& v1, const Vector4& v2)
{
    Vector3 ab = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
    Vector3 ac = { v2.x - v0.x, v2.y - v0.y, v2.z - v0.z };
    Vector3 normal = CrossProduct(ab, ac);
    float length = Length(normal);
    if (length != 0.0f)
    {
        normal.x /= length;
        normal.y /= length;
        normal.z /= length;
    }
    return normal;
}

Vector3 Normalize(const Vector3& v)
{
    Vector3 Return{};
    float length = Length(v);

    Return.x = v.x / length;
    Return.y = v.y / length;
    Return.z = v.z / length;

    return Return;
}


Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix)
{
    Vector3 result{};

    // ベクトルと行列の計算
    result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
    result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
    result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
    float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

    // wが0でなければ
    assert(w != 0.0f);
    result.x /= w;
    result.y /= w;
    result.z /= w;

    // 計算結果をVector3型で返す
    return result;
}

Vector4 Transform(const Vector4& v, const Matrix4x4& m)
{
    Vector4 result;
    result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
    result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
    result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
    result.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];
    return result;
}

#pragma endregion

#pragma region Matrix4*4


Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2)
{
    Matrix4x4 Return{};

    Return.m[0][0] = m1.m[0][0] + m2.m[0][0];
    Return.m[1][0] = m1.m[1][0] + m2.m[1][0];
    Return.m[2][0] = m1.m[2][0] + m2.m[2][0];
    Return.m[3][0] = m1.m[3][0] + m2.m[3][0];
    Return.m[0][1] = m1.m[0][1] + m2.m[0][1];
    Return.m[1][1] = m1.m[1][1] + m2.m[1][1];
    Return.m[2][1] = m1.m[2][1] + m2.m[2][1];
    Return.m[3][1] = m1.m[3][1] + m2.m[3][1];
    Return.m[0][2] = m1.m[0][2] + m2.m[0][2];
    Return.m[1][2] = m1.m[1][2] + m2.m[1][2];
    Return.m[2][2] = m1.m[2][2] + m2.m[2][2];
    Return.m[3][2] = m1.m[3][2] + m2.m[3][2];
    Return.m[0][3] = m1.m[0][3] + m2.m[0][3];
    Return.m[1][3] = m1.m[1][3] + m2.m[1][3];
    Return.m[2][3] = m1.m[2][3] + m2.m[2][3];
    Return.m[3][3] = m1.m[3][3] + m2.m[3][3];

    return Return;
}

Matrix4x4 Sub(const Matrix4x4& m1, const Matrix4x4& m2)
{
    Matrix4x4 Return{};

    Return.m[0][0] = m1.m[0][0] - m2.m[0][0];
    Return.m[1][0] = m1.m[1][0] - m2.m[1][0];
    Return.m[2][0] = m1.m[2][0] - m2.m[2][0];
    Return.m[3][0] = m1.m[3][0] - m2.m[3][0];
    Return.m[0][1] = m1.m[0][1] - m2.m[0][1];
    Return.m[1][1] = m1.m[1][1] - m2.m[1][1];
    Return.m[2][1] = m1.m[2][1] - m2.m[2][1];
    Return.m[3][1] = m1.m[3][1] - m2.m[3][1];
    Return.m[0][2] = m1.m[0][2] - m2.m[0][2];
    Return.m[1][2] = m1.m[1][2] - m2.m[1][2];
    Return.m[2][2] = m1.m[2][2] - m2.m[2][2];
    Return.m[3][2] = m1.m[3][2] - m2.m[3][2];
    Return.m[0][3] = m1.m[0][3] - m2.m[0][3];
    Return.m[1][3] = m1.m[1][3] - m2.m[1][3];
    Return.m[2][3] = m1.m[2][3] - m2.m[2][3];
    Return.m[3][3] = m1.m[3][3] - m2.m[3][3];

    return Return;
}

Matrix4x4 Mul(const Matrix4x4& m1, const Matrix4x4& m2)
{
    Matrix4x4 Return{};

    Return.m[0][0] = (m1.m[0][0] * m2.m[0][0]) + (m1.m[0][1] * m2.m[1][0]) + (m1.m[0][2] * m2.m[2][0]) + (m1.m[0][3] * m2.m[3][0]);
    Return.m[1][0] = (m1.m[1][0] * m2.m[0][0]) + (m1.m[1][1] * m2.m[1][0]) + (m1.m[1][2] * m2.m[2][0]) + (m1.m[1][3] * m2.m[3][0]);
    Return.m[2][0] = (m1.m[2][0] * m2.m[0][0]) + (m1.m[2][1] * m2.m[1][0]) + (m1.m[2][2] * m2.m[2][0]) + (m1.m[2][3] * m2.m[3][0]);
    Return.m[3][0] = (m1.m[3][0] * m2.m[0][0]) + (m1.m[3][1] * m2.m[1][0]) + (m1.m[3][2] * m2.m[2][0]) + (m1.m[3][3] * m2.m[3][0]);
    Return.m[0][1] = (m1.m[0][0] * m2.m[0][1]) + (m1.m[0][1] * m2.m[1][1]) + (m1.m[0][2] * m2.m[2][1]) + (m1.m[0][3] * m2.m[3][1]);
    Return.m[1][1] = (m1.m[1][0] * m2.m[0][1]) + (m1.m[1][1] * m2.m[1][1]) + (m1.m[1][2] * m2.m[2][1]) + (m1.m[1][3] * m2.m[3][1]);
    Return.m[2][1] = (m1.m[2][0] * m2.m[0][1]) + (m1.m[2][1] * m2.m[1][1]) + (m1.m[2][2] * m2.m[2][1]) + (m1.m[2][3] * m2.m[3][1]);
    Return.m[3][1] = (m1.m[3][0] * m2.m[0][1]) + (m1.m[3][1] * m2.m[1][1]) + (m1.m[3][2] * m2.m[2][1]) + (m1.m[3][3] * m2.m[3][1]);
    Return.m[0][2] = (m1.m[0][0] * m2.m[0][2]) + (m1.m[0][1] * m2.m[1][2]) + (m1.m[0][2] * m2.m[2][2]) + (m1.m[0][3] * m2.m[3][2]);
    Return.m[1][2] = (m1.m[1][0] * m2.m[0][2]) + (m1.m[1][1] * m2.m[1][2]) + (m1.m[1][2] * m2.m[2][2]) + (m1.m[1][3] * m2.m[3][2]);
    Return.m[2][2] = (m1.m[2][0] * m2.m[0][2]) + (m1.m[2][1] * m2.m[1][2]) + (m1.m[2][2] * m2.m[2][2]) + (m1.m[2][3] * m2.m[3][2]);
    Return.m[3][2] = (m1.m[3][0] * m2.m[0][2]) + (m1.m[3][1] * m2.m[1][2]) + (m1.m[3][2] * m2.m[2][2]) + (m1.m[3][3] * m2.m[3][2]);
    Return.m[0][3] = (m1.m[0][0] * m2.m[0][3]) + (m1.m[0][1] * m2.m[1][3]) + (m1.m[0][2] * m2.m[2][3]) + (m1.m[0][3] * m2.m[3][3]);
    Return.m[1][3] = (m1.m[1][0] * m2.m[0][3]) + (m1.m[1][1] * m2.m[1][3]) + (m1.m[1][2] * m2.m[2][3]) + (m1.m[1][3] * m2.m[3][3]);
    Return.m[2][3] = (m1.m[2][0] * m2.m[0][3]) + (m1.m[2][1] * m2.m[1][3]) + (m1.m[2][2] * m2.m[2][3]) + (m1.m[2][3] * m2.m[3][3]);
    Return.m[3][3] = (m1.m[3][0] * m2.m[0][3]) + (m1.m[3][1] * m2.m[1][3]) + (m1.m[3][2] * m2.m[2][3]) + (m1.m[3][3] * m2.m[3][3]);

    return Return;
}

Matrix4x4 Inverse(const Matrix4x4& m)
{
    Matrix4x4 Return{};

    float A = (m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]) + (m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]) + (m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2])
        - (m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2])
        - (m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2])
        + (m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]) + (m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]) + (m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2])
        + (m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]) + (m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]) + (m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2])
        - (m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2])
        - (m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0])
        + (m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]) + (m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]) + (m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0]);

    Return.m[0][0] = (1 / A) * ((m.m[1][1] * m.m[2][2] * m.m[3][3]) + (m.m[1][2] * m.m[2][3] * m.m[3][1]) + (m.m[1][3] * m.m[2][1] * m.m[3][2]) - (m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[1][1] * m.m[2][3] * m.m[3][2]));
    Return.m[0][1] = (1 / A) * ((m.m[0][3] * m.m[2][2] * m.m[3][1]) + (m.m[0][2] * m.m[2][1] * m.m[3][3]) + (m.m[0][1] * m.m[2][3] * m.m[3][2]) - (m.m[0][1] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[2][1] * m.m[3][2]));
    Return.m[0][2] = (1 / A) * ((m.m[0][1] * m.m[1][2] * m.m[3][3]) + (m.m[0][2] * m.m[1][3] * m.m[3][1]) + (m.m[0][3] * m.m[1][1] * m.m[3][2]) - (m.m[0][3] * m.m[1][2] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[3][2]));
    Return.m[0][3] = (1 / A) * ((m.m[0][3] * m.m[1][2] * m.m[2][1]) + (m.m[0][2] * m.m[1][1] * m.m[2][3]) + (m.m[0][1] * m.m[1][3] * m.m[2][2]) - (m.m[0][1] * m.m[1][2] * m.m[2][3]) - (m.m[0][2] * m.m[1][3] * m.m[2][1]) - (m.m[0][3] * m.m[1][1] * m.m[2][2]));

    Return.m[1][0] = (1 / A) * ((m.m[1][3] * m.m[2][2] * m.m[3][0]) + (m.m[1][2] * m.m[2][0] * m.m[3][3]) + (m.m[1][0] * m.m[2][3] * m.m[3][2]) - (m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[1][3] * m.m[2][0] * m.m[3][2]));
    Return.m[1][1] = (1 / A) * ((m.m[0][0] * m.m[2][2] * m.m[3][3]) + (m.m[0][2] * m.m[2][3] * m.m[3][0]) + (m.m[0][3] * m.m[2][0] * m.m[3][2]) - (m.m[0][3] * m.m[2][2] * m.m[3][0]) - (m.m[0][2] * m.m[2][0] * m.m[3][3]) - (m.m[0][0] * m.m[2][3] * m.m[3][2]));
    Return.m[1][2] = (1 / A) * ((m.m[0][3] * m.m[1][2] * m.m[3][0]) + (m.m[0][2] * m.m[1][0] * m.m[3][3]) + (m.m[0][0] * m.m[1][3] * m.m[3][2]) - (m.m[0][0] * m.m[1][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][3] * m.m[3][0]) - (m.m[0][3] * m.m[1][0] * m.m[3][2]));
    Return.m[1][3] = (1 / A) * ((m.m[0][0] * m.m[1][2] * m.m[2][3]) + (m.m[0][2] * m.m[1][3] * m.m[2][0]) + (m.m[0][3] * m.m[1][0] * m.m[2][2]) - (m.m[0][3] * m.m[1][2] * m.m[2][0]) - (m.m[0][2] * m.m[1][0] * m.m[2][3]) - (m.m[0][0] * m.m[1][3] * m.m[2][2]));

    Return.m[2][0] = (1 / A) * ((m.m[1][0] * m.m[2][1] * m.m[3][3]) + (m.m[1][1] * m.m[2][3] * m.m[3][0]) + (m.m[1][3] * m.m[2][0] * m.m[3][1]) - (m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[1][0] * m.m[2][3] * m.m[3][1]));
    Return.m[2][1] = (1 / A) * ((m.m[0][3] * m.m[2][1] * m.m[3][0]) + (m.m[0][1] * m.m[2][0] * m.m[3][3]) + (m.m[0][0] * m.m[2][3] * m.m[3][1]) - (m.m[0][0] * m.m[2][1] * m.m[3][3]) - (m.m[0][1] * m.m[2][3] * m.m[3][0]) - (m.m[0][3] * m.m[2][0] * m.m[3][1]));
    Return.m[2][2] = (1 / A) * ((m.m[0][0] * m.m[1][1] * m.m[3][3]) + (m.m[0][1] * m.m[1][3] * m.m[3][0]) + (m.m[0][3] * m.m[1][0] * m.m[3][1]) - (m.m[0][3] * m.m[1][1] * m.m[3][0]) - (m.m[0][1] * m.m[1][0] * m.m[3][3]) - (m.m[0][0] * m.m[1][3] * m.m[3][1]));
    Return.m[2][3] = (1 / A) * ((m.m[0][3] * m.m[1][1] * m.m[2][0]) + (m.m[0][1] * m.m[1][0] * m.m[2][3]) + (m.m[0][0] * m.m[1][3] * m.m[2][1]) - (m.m[0][0] * m.m[1][1] * m.m[2][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0]) - (m.m[0][3] * m.m[1][0] * m.m[2][1]));

    Return.m[3][0] = (1 / A) * ((m.m[1][2] * m.m[2][1] * m.m[3][0]) + (m.m[1][1] * m.m[2][0] * m.m[3][2]) + (m.m[1][0] * m.m[2][2] * m.m[3][1]) - (m.m[1][0] * m.m[2][1] * m.m[3][2]) - (m.m[1][1] * m.m[2][2] * m.m[3][0]) - (m.m[1][2] * m.m[2][0] * m.m[3][1]));
    Return.m[3][1] = (1 / A) * ((m.m[0][0] * m.m[2][1] * m.m[3][2]) + (m.m[0][1] * m.m[2][2] * m.m[3][0]) + (m.m[0][2] * m.m[2][0] * m.m[3][1]) - (m.m[0][2] * m.m[2][1] * m.m[3][0]) - (m.m[0][1] * m.m[2][0] * m.m[3][2]) - (m.m[0][0] * m.m[2][2] * m.m[3][1]));
    Return.m[3][2] = (1 / A) * ((m.m[0][2] * m.m[1][1] * m.m[3][0]) + (m.m[0][1] * m.m[1][0] * m.m[3][2]) + (m.m[0][0] * m.m[1][2] * m.m[3][1]) - (m.m[0][0] * m.m[1][1] * m.m[3][2]) - (m.m[0][1] * m.m[1][2] * m.m[3][0]) - (m.m[0][2] * m.m[1][0] * m.m[3][1]));
    Return.m[3][3] = (1 / A) * ((m.m[0][0] * m.m[1][1] * m.m[2][2]) + (m.m[0][1] * m.m[1][2] * m.m[2][0]) + (m.m[0][2] * m.m[1][0] * m.m[2][1]) - (m.m[0][2] * m.m[1][1] * m.m[2][0]) - (m.m[0][1] * m.m[1][0] * m.m[2][2]) - (m.m[0][0] * m.m[1][2] * m.m[2][1]));

    return Return;
}

Matrix4x4 Transpose(const Matrix4x4& m)
{
    Matrix4x4 Return{};

    Return.m[0][0] = m.m[0][0];
    Return.m[1][0] = m.m[0][1];
    Return.m[2][0] = m.m[0][2];
    Return.m[3][0] = m.m[0][3];
    Return.m[0][1] = m.m[1][0];
    Return.m[1][1] = m.m[1][1];
    Return.m[2][1] = m.m[1][2];
    Return.m[3][1] = m.m[1][3];
    Return.m[0][2] = m.m[2][0];
    Return.m[1][2] = m.m[2][1];
    Return.m[2][2] = m.m[2][2];
    Return.m[3][2] = m.m[2][3];
    Return.m[0][3] = m.m[3][0];
    Return.m[1][3] = m.m[3][1];
    Return.m[2][3] = m.m[3][2];
    Return.m[3][3] = m.m[3][3];

    return Return;
}

Matrix4x4 MakeIdentity4x4()
{
    Matrix4x4 Return{};

    Return.m[0][0] = 1;
    Return.m[1][0] = 0;
    Return.m[2][0] = 0;
    Return.m[3][0] = 0;
    Return.m[0][1] = 0;
    Return.m[1][1] = 1;
    Return.m[2][1] = 0;
    Return.m[3][1] = 0;
    Return.m[0][2] = 0;
    Return.m[1][2] = 0;
    Return.m[2][2] = 1;
    Return.m[3][2] = 0;
    Return.m[0][3] = 0;
    Return.m[1][3] = 0;
    Return.m[2][3] = 0;
    Return.m[3][3] = 1;

    return Return;
}

Matrix4x4 MakeTranslateMatrix(const Vector3& translate)
{
    Matrix4x4 returnMatrix{};

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            returnMatrix.m[i][j] = { 0 };
        }
    }

    returnMatrix.m[0][0] = { 1 };
    returnMatrix.m[1][1] = { 1 };
    returnMatrix.m[2][2] = { 1 };
    returnMatrix.m[3][3] = { 1 };

    returnMatrix.m[3][0] = { translate.x };
    returnMatrix.m[3][1] = { translate.y };
    returnMatrix.m[3][2] = { translate.z };


    return returnMatrix;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale)
{
    Matrix4x4 returnMatrix{};

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            returnMatrix.m[i][j] = 0.0f;
        }
    }
    returnMatrix.m[0][0] = scale.x;
    returnMatrix.m[1][1] = scale.y;
    returnMatrix.m[2][2] = scale.z;
    returnMatrix.m[3][3] = 1.0f;

    return returnMatrix;
}

// Ｘ軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian)
{
    Matrix4x4 Return{};

    Return.m[0][0] = 1;
    Return.m[1][0] = 0;
    Return.m[2][0] = 0;
    Return.m[3][0] = 0;
    Return.m[0][1] = 0;
    Return.m[1][1] = std::cos(radian);
    Return.m[2][1] = -std::sin(radian);
    Return.m[3][1] = 0;
    Return.m[0][2] = 0;
    Return.m[1][2] = std::sin(radian);
    Return.m[2][2] = std::cos(radian);
    Return.m[3][2] = 0;
    Return.m[0][3] = 0;
    Return.m[1][3] = 0;
    Return.m[2][3] = 0;
    Return.m[3][3] = 1;

    return Return;
}

// Ｙ軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian)
{
    Matrix4x4 Return{};

    Return.m[0][0] = std::cos(radian);
    Return.m[1][0] = 0;
    Return.m[2][0] = std::sin(radian);
    Return.m[3][0] = 0;
    Return.m[0][1] = 0;
    Return.m[1][1] = 1;
    Return.m[2][1] = 0;
    Return.m[3][1] = 0;
    Return.m[0][2] = -std::sin(radian);
    Return.m[1][2] = 0;
    Return.m[2][2] = std::cos(radian);
    Return.m[3][2] = 0;
    Return.m[0][3] = 0;
    Return.m[1][3] = 0;
    Return.m[2][3] = 0;
    Return.m[3][3] = 1;

    return Return;
}

// Ｚ軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian)
{
    Matrix4x4 Return{};

    Return.m[0][0] = std::cos(radian);
    Return.m[1][0] = -std::sin(radian);
    Return.m[2][0] = 0;
    Return.m[3][0] = 0;
    Return.m[0][1] = std::sin(radian);
    Return.m[1][1] = std::cos(radian);
    Return.m[2][1] = 0;
    Return.m[3][1] = 0;
    Return.m[0][2] = 0;
    Return.m[1][2] = 0;
    Return.m[2][2] = 1;
    Return.m[3][2] = 0;
    Return.m[0][3] = 0;
    Return.m[1][3] = 0;
    Return.m[2][3] = 0;
    Return.m[3][3] = 1;

    return Return;
}

// ３次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate)
{
    Matrix4x4 Return{};

    Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
    Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
    Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
    Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
    Matrix4x4 rotateXYZMatrix = Mul(Mul(rotateZMatrix, rotateXMatrix), rotateYMatrix);
    Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
    Matrix4x4 resultMatrix = Mul(Mul(scaleMatrix, rotateXYZMatrix), translateMatrix);


    return resultMatrix;
}

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
    Matrix4x4 Return{};
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            Return.m[i][j] = 0.0f;
        }
    }

    Return.m[0][0] = (1 / aspectRatio) * (1 / (tanf(fovY / 2)));
    Return.m[1][1] = (1 / (tanf(fovY / 2)));
    Return.m[2][2] = farClip / (farClip - nearClip);
    Return.m[3][2] = (-1 * nearClip * farClip) / (farClip - nearClip);
    Return.m[2][3] = 1;


    return Return;
}

// 正射影行列(平行投影行列)
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
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
    Return.m[2][1] = 1.0f / (farClip - nearClip);
    Return.m[2][2] = 0.0f;
    Return.m[2][3] = 0.0f;

    Return.m[3][0] = (left + right) / (left - right);
    Return.m[3][1] = (top + bottom) / (bottom - top);
    Return.m[3][2] = (nearClip) / (nearClip - farClip);
    Return.m[3][3] = 1.0f;

    return Return;
}

// ビューポート変換
Matrix4x4 MakeViewPortMatrix(float left, float top, float width, float height, float minD, float maxD)
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


#pragma region collision

bool IsCollision(const Sphere& s1, const Sphere& s2)
{
    Vector3 gappoint;
    gappoint.x = s1.center.x - s2.center.x;
    gappoint.y = s1.center.y - s2.center.y;
    gappoint.z = s1.center.z - s2.center.z;

    float gap = Length(gappoint);

    float i = s1.radius + s2.radius;

    if (i < gap)return false;
    else return true;
}

bool IsCollision(const Sphere& s, const Plane& p)
{
    // 球の中心から平面までの距離を計算
    float dist = DotProduct(s.center, p.normal) - p.distance;
    // 距離の絶対値が半径以下なら衝突
    return std::abs(dist) <= s.radius;
}

bool IsCollision(const Segment& s, const Plane& p)
{
    // 線分の始点と終点
    const Vector3& start = s.origin;
    const Vector3& end = Add(s.origin, s.diff);

    // 始点と終点が平面のどちら側にあるかを判定　この数字が０になると、平面上にあるということになる
    float distStart = DotProduct(start, p.normal) - p.distance;
    float distEnd = DotProduct(end, p.normal) - p.distance;

    // 始点と終点が平面の表裏にあるなら（distStartとdistEndの組み合わせが０以下と以上）交差してる
    if (distStart * distEnd <= 0.0f)
    {
        return true;
    }
    return false;
}

bool IsCollision(const Segment& s, const Triangle& t)
{
    // 三角形の法線と平面の距離を求める
    Vector3 edge1 = Sub(t.vertices[1], t.vertices[0]);
    Vector3 edge2 = Sub(t.vertices[2], t.vertices[0]);
    Vector3 normal = Normalize(CrossProduct(edge1, edge2));
    float distance = DotProduct(normal, t.vertices[0]);

    // １，線と三角形の存在する平面の衝突判定
    if (!IsCollision(s, Plane{ normal, distance }))
    {
        return false;
    }

    // bool IsCollision(const Segment & s, const Plane & p)より
    // 線分の始点と終点
    Vector3 start = s.origin;
    Vector3 end = Add(s.origin, s.diff);

    // 線分と平面の交点を求める
    float distStart = DotProduct(start, normal) - distance;
    float distEnd = DotProduct(end, normal) - distance;
    float tParam = distStart / (distStart - distEnd);
    // 衝突点
    Vector3 intersect = Add(start, Mul(tParam, Sub(end, start)));

    // 各辺と交点のクロス積で判定
    bool allSame = true;
    float sign = 0.0f;
    for (int i = 0; i < 3; ++i)
    {
        // 始点
        Vector3 v0 = t.vertices[i];
        // 終点
        Vector3 v1 = t.vertices[(i + 1) % 3];
        // 始点と終点のベクトル
        Vector3 edge = Sub(v1, v0);
        // 始点と衝突点のベクトル
        Vector3 toP = Sub(intersect, v0);
        // 上記２つのクロス積
        Vector3 cross = CrossProduct(edge, toP);
        // 三角形の法線とクロス積の内積
        float dot = DotProduct(normal, cross);
        // 1つ目の三角形の向きを基準にして2,3つ目の向きと比較する
        if (i == 0)
        {
            sign = dot;
        }
        else
        {
            // 向きの不一致が起きた
            if (sign * dot < 0.0f)
            {
                allSame = false;
                break;
            }
        }
    }
    return allSame;
}

bool IsCollision(const Ray& r, const Plane& p)
{
    // 線分の始点と方向
    const Vector3& start = r.origin;
    const Vector3& dir = r.diff;

    // 面の法線と原点らの距離
    const Vector3& normal = p.normal;
    float distance = p.distance;

    // レイの方向と平面法線の内積
    float denom = DotProduct(dir, p.normal);

    // レイが平面と平行なら衝突しない
    if (std::abs(denom) < 1e-6f)
    {
        return false;
    }

    // レイの始点が平面より手前にあるか
    float t = (p.distance - DotProduct(start, normal)) / denom;

    if (t < 0.0f)
    {
        return false;
    }

    return true;
}

bool IsCollision(const Ray& r, const AABB& aabb)
{
    float tmin = (aabb.min.x - r.origin.x) / r.diff.x;
    float tmax = (aabb.max.x - r.origin.x) / r.diff.x;
    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (aabb.min.y - r.origin.y) / r.diff.y;
    float tymax = (aabb.max.y - r.origin.y) / r.diff.y;
    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (aabb.min.z - r.origin.z) / r.diff.z;
    float tzmax = (aabb.max.z - r.origin.z) / r.diff.z;
    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    return true;
}

bool IsCollision(const Ray& r, const Triangle& t)
{
    // 三角形の法線と平面の距離を求める
    Vector3 edge1 = Sub(t.vertices[1], t.vertices[0]);
    Vector3 edge2 = Sub(t.vertices[2], t.vertices[0]);
    Vector3 normal = Normalize(CrossProduct(edge1, edge2));
    float distance = DotProduct(normal, t.vertices[0]);

    // １，線と三角形の存在する平面の衝突判定
    if (!IsCollision(r, Plane{ normal, distance }))
    {
        return false;
    }

    // 線分の始点と方向
    const Vector3& start = r.origin;
    const Vector3& dir = r.diff;

    // 線分と平面の交点を求める
    float denom = DotProduct(dir, normal);
    float tParam = (distance - DotProduct(start, normal)) / denom;
    // 衝突点
    Vector3 intersect = Add(start, Mul(tParam, dir));

    // 各辺と交点のクロス積で判定
    bool allSame = true;
    float sign = 0.0f;
    for (int i = 0; i < 3; ++i)
    {
        // 始点
        Vector3 v0 = t.vertices[i];
        // 終点
        Vector3 v1 = t.vertices[(i + 1) % 3];
        // 始点と終点のベクトル
        Vector3 edge = Sub(v1, v0);
        // 始点と衝突点のベクトル
        Vector3 toP = Sub(intersect, v0);
        // 上記２つのクロス積
        Vector3 cross = CrossProduct(edge, toP);
        // 三角形の法線とクロス積の内積
        float dot = DotProduct(normal, cross);
        // 1つ目の三角形の向きを基準にして2,3つ目の向きと比較する
        if (i == 0)
        {
            sign = dot;
        }
        else
        {
            // 向きの不一致が起きた
            if (sign * dot < 1e-6f)
            {
                allSame = false;
                break;
            }
        }
    }
    return allSame;
}

// モデルのAABBと三角形配列で詳細判定
bool IsCollision(const Ray& ray, const AABB& aabb, const std::vector<VertexData>& vertices, const Matrix4x4& worldMatrix)
{
    // まずAABBで大まかに判定
    if (!IsCollision(ray, aabb))
    {
        return false;
    }
    // 下の行を消して修正を再開
    return true;

    // AABBに当たっていた場合のみ、三角形ごとに詳細判定
    for (size_t i = 0; i + 2 < vertices.size(); i += 3)
    {
        Triangle t;
        // 三角形の頂点をワールド座標に変換
        t.vertices[0] = Transform(
            Vector3{ vertices[i].position.x, vertices[i].position.y, vertices[i].position.z },
            worldMatrix
        );
        t.vertices[1] = Transform(
            Vector3{ vertices[i + 1].position.x, vertices[i + 1].position.y, vertices[i + 1].position.z },
            worldMatrix
        );
        t.vertices[2] = Transform(
            Vector3{ vertices[i + 2].position.x, vertices[i + 2].position.y, vertices[i + 2].position.z },
            worldMatrix
        );


        if (IsCollision(ray, t))
        {
            return true; // どれか1つでも当たればtrue
        }
    }
    return false;
}



// ローカルAABB（中心0, サイズ1）をワールド行列で変換し、ワールドAABBを返す
AABB CreateAABB(const Transforms& transforms)
{
    Matrix4x4 worldMatrix = MakeAffineMatrix(transforms.scale, transforms.rotate, transforms.translate);

    // ローカルAABBの8頂点
    Vector3 localMin = { -0.5f, -0.5f, -0.5f };
    Vector3 localMax = { 0.5f,  0.5f,  0.5f };
    Vector3 corners[8] = {
        {localMin.x, localMin.y, localMin.z},
        {localMax.x, localMin.y, localMin.z},
        {localMin.x, localMax.y, localMin.z},
        {localMax.x, localMax.y, localMin.z},
        {localMin.x, localMin.y, localMax.z},
        {localMax.x, localMin.y, localMax.z},
        {localMin.x, localMax.y, localMax.z},
        {localMax.x, localMax.y, localMax.z}
    };

    // 8頂点をワールド空間に変換
    Vector3 worldMin = Transform(corners[0], worldMatrix);
    Vector3 worldMax = worldMin;
    for (int i = 1; i < 8; ++i)
    {
        Vector3 v = Transform(corners[i], worldMatrix);
        worldMin.x = my_min(worldMin.x, v.x);
        worldMin.y = my_min(worldMin.y, v.y);
        worldMin.z = my_min(worldMin.z, v.z);
        worldMax.x = my_max(worldMax.x, v.x);
        worldMax.y = my_max(worldMax.y, v.y);
        worldMax.z = my_max(worldMax.z, v.z);
    }
    return { worldMin, worldMax };
}


#pragma endregion


void CreateSphere(VertexData* vertexData, uint32_t kSubdivision)
{
    if (kSubdivision == 0 || vertexData == nullptr)
    {
        return;
    }

    // 経度分割１つ分の角度
    const float kLonEvery = float((2 * M_PI) / kSubdivision);
    // 緯度分割１つ分の角度
    const float kLatEvery = float(M_PI / kSubdivision);

    // 緯度の方向に分割 -π/2 ～ π/2
    for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
    {
        // 現在の緯度と次の緯度
        const float lat = float(-M_PI / 2.0f + latIndex * kLatEvery);
        const float nextLat = lat + kLatEvery;

        // 経度方向に分割 0 ～ 2π
        for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
        {
            // 現在の経度と次の経度
            const float lon = lonIndex * kLonEvery;
            const float nextLon = (lonIndex + 1) * kLonEvery;

            // テクスチャ座標を計算（[0,1]範囲をしっかりカバー）
            const float u = float(lonIndex) / float(kSubdivision);
            const float nextU = float(lonIndex + 1) / float(kSubdivision);
            const float v = 1.0f - float(latIndex) / float(kSubdivision);
            const float nextV = 1.0f - float(latIndex + 1) / float(kSubdivision);

            // 頂点データの開始インデックス
            const uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;

            // 頂点データを設定 (三角形1)
            vertexData[start + 0].position = { std::cos(lat) * std::cos(lon), std::sin(lat), std::cos(lat) * std::sin(lon), 1.0f };
            vertexData[start + 0].texcoord = { u, v };

            vertexData[start + 2].position = { std::cos(nextLat) * std::cos(nextLon), std::sin(nextLat), std::cos(nextLat) * std::sin(nextLon), 1.0f };
            vertexData[start + 2].texcoord = { nextU, nextV };

            vertexData[start + 1].position = { std::cos(nextLat) * std::cos(lon), std::sin(nextLat), std::cos(nextLat) * std::sin(lon), 1.0f };
            vertexData[start + 1].texcoord = { u, nextV };

            // 頂点データを設定 (三角形2)
            vertexData[start + 3].position = { std::cos(lat) * std::cos(lon), std::sin(lat), std::cos(lat) * std::sin(lon), 1.0f };
            vertexData[start + 3].texcoord = { u, v };

            vertexData[start + 5].position = { std::cos(lat) * std::cos(nextLon), std::sin(lat), std::cos(lat) * std::sin(nextLon), 1.0f };
            vertexData[start + 5].texcoord = { nextU, v };

            vertexData[start + 4].position = { std::cos(nextLat) * std::cos(nextLon), std::sin(nextLat), std::cos(nextLat) * std::sin(nextLon), 1.0f };
            vertexData[start + 4].texcoord = { nextU, nextV };

            // 法線を正規化して設定
            for (int i = 0; i < 6; ++i)
            {
                Vector3 n = {
                    vertexData[start + i].position.x,
                    vertexData[start + i].position.y,
                    vertexData[start + i].position.z
                };
                vertexData[start + i].normal = Normalize(n);
            }
        }
    }
}

// 文字列変換
std::wstring ConvertString(const std::string& str)
{
    if (str.empty())
    {
        return std::wstring();
    }

    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if (sizeNeeded == 0)
    {
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

// 文字列変換
std::string ConvertString(const std::wstring& str)
{
    if (str.empty())
    {
        return std::string();
    }

    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
    if (sizeNeeded == 0)
    {
        return std::string();
    }
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
    return result;
}

// D3D12_RESOURCE_STATES を文字列に変換する関数
std::string ResourceStateToString(D3D12_RESOURCE_STATES state)
{
    switch (state)
    {
    case D3D12_RESOURCE_STATE_COMMON: return "COMMON";
    case D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER: return "VERTEX_AND_CONSTANT_BUFFER";
    case D3D12_RESOURCE_STATE_INDEX_BUFFER: return "INDEX_BUFFER";
    case D3D12_RESOURCE_STATE_RENDER_TARGET: return "RENDER_TARGET";
    case D3D12_RESOURCE_STATE_UNORDERED_ACCESS: return "UNORDERED_ACCESS";
    case D3D12_RESOURCE_STATE_DEPTH_WRITE: return "DEPTH_WRITE";
    case D3D12_RESOURCE_STATE_DEPTH_READ: return "DEPTH_READ";
    case D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE: return "NON_PIXEL_SHADER_RESOURCE";
    case D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE: return "PIXEL_SHADER_RESOURCE";
    case D3D12_RESOURCE_STATE_STREAM_OUT: return "STREAM_OUT";
    case D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT: return "INDIRECT_ARGUMENT";
    case D3D12_RESOURCE_STATE_COPY_DEST: return "COPY_DEST";
    case D3D12_RESOURCE_STATE_COPY_SOURCE: return "COPY_SOURCE";
    case D3D12_RESOURCE_STATE_RESOLVE_DEST: return "RESOLVE_DEST";
    case D3D12_RESOURCE_STATE_RESOLVE_SOURCE: return "RESOLVE_SOURCE";
    case D3D12_RESOURCE_STATE_VIDEO_DECODE_READ: return "VIDEO_DECODE_READ";
    case D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE: return "VIDEO_DECODE_WRITE";
    case D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ: return "VIDEO_PROCESS_READ";
    case D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE: return "VIDEO_PROCESS_WRITE";
    case D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ: return "VIDEO_ENCODE_READ";
    case D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE: return "VIDEO_ENCODE_WRITE";
    default: return "UNKNOWN_STATE";
    }
}

// ログを出す関数
void Log(const std::string& message)
{
    // string型からchar*型に変換した文字列
    OutputDebugStringA(message.c_str());
}
// Vector4型用のオーバーロード
void Log(const std::string& message, const Vector4& vector)
{
    Log(message);
    std::string a = std::format("x={}, y={}, z={}, w={}", vector.x, vector.y, vector.z, vector.w);
    Log(a);
}
// Matrix4x4型用のオーバーロード
void Log(const std::string& message, const Matrix4x4& matrix)
{
    Log(message);
    std::string a = ":\n";
    for (int i = 0; i < 4; ++i)
    {
        a += std::format("[{}, {}, {}, {}]\n", matrix.m[i][0], matrix.m[i][1], matrix.m[i][2], matrix.m[i][3]);
    }
    Log(a);
}
// barrier.Transitionの状態をログに出力する関数
void Log(const std::string& message, const D3D12_RESOURCE_BARRIER& barrier)
{
    Log(message);
    if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
    {
        std::string stateBefore = ResourceStateToString(barrier.Transition.StateBefore);
        std::string stateAfter = ResourceStateToString(barrier.Transition.StateAfter);
        std::string a = std::format("Barrier Transition - StateBefore: {}, StateAfter: {}", stateBefore, stateAfter);
        Log(a);
    }
    else
    {
        Log("Barrier is not of type TRANSITION.");
    }
}
// RootSignatureの状態をログに出力する関数
void Log(const D3D12_ROOT_SIGNATURE_DESC& desc)
{
    std::ostringstream oss;
    oss << "[Root Signature]\n"
        << "NumParameters: " << desc.NumParameters << "\n"
        << "NumStaticSamplers: " << desc.NumStaticSamplers << "\n"
        << "Flags: " << desc.Flags << "\n";

    Log(oss.str());

    for (UINT i = 0; i < desc.NumParameters; ++i)
    {
        const auto& param = desc.pParameters[i];
        oss.str(""); // バッファをクリア
        oss.clear(); // 状態をリセット
        oss << "[Root Parameter " << i << "]\n"
            << "Type: " << param.ParameterType << "\n"
            << "ShaderVisibility: " << param.ShaderVisibility << "\n";
        Log(oss.str());

        if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV ||
            param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV ||
            param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_UAV)
        {
            oss.str("");
            oss.clear();
            oss << "ShaderRegister: " << param.Descriptor.ShaderRegister << "\n"
                << "RegisterSpace: " << param.Descriptor.RegisterSpace << "\n";
            Log(oss.str());
        }
        else if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
            oss.str("");
            oss.clear();
            oss << "NumDescriptorRanges: " << param.DescriptorTable.NumDescriptorRanges << "\n";
            Log(oss.str());
            for (UINT j = 0; j < param.DescriptorTable.NumDescriptorRanges; ++j)
            {
                const auto& range = param.DescriptorTable.pDescriptorRanges[j];
                oss.str("");
                oss.clear();
                oss << "  [Descriptor Range " << j << "]\n"
                    << "  RangeType: " << range.RangeType << "\n"
                    << "  BaseShaderRegister: " << range.BaseShaderRegister << "\n"
                    << "  NumDescriptors: " << range.NumDescriptors << "\n"
                    << "  RegisterSpace: " << range.RegisterSpace << "\n";
                Log(oss.str());
            }
        }
    }

    for (UINT i = 0; i < desc.NumStaticSamplers; ++i)
    {
        const auto& sampler = desc.pStaticSamplers[i];
        oss.str("");
        oss.clear();
        oss << "[Static Sampler " << i << "]\n"
            << "ShaderRegister: " << sampler.ShaderRegister << "\n"
            << "Filter: " << sampler.Filter << "\n"
            << "AddressU: " << sampler.AddressU << "\n"
            << "AddressV: " << sampler.AddressV << "\n"
            << "AddressW: " << sampler.AddressW << "\n"
            << "ShaderVisibility: " << sampler.ShaderVisibility << "\n";
        Log(oss.str());
    }
}

// ログをファイルに書き出す
void Log(std::ofstream& os, const std::string& message)
{
    os << message << std::endl;
    OutputDebugStringA(message.c_str());
}

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label)
{
    // ここではデバッグ出力に表示します（実際の画面描画は環境依存）
    char buffer[256];
    sprintf_s(buffer, "%s: (%.3f, %.3f, %.3f)\n", label, vector.x, vector.y, vector.z);
    OutputDebugStringA(buffer);
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label)
{
    // ここではデバッグ出力に表示します（実際の画面描画は環境依存）
    char buffer[256];
    OutputDebugStringA(label);
    OutputDebugStringA(":\n");
    for (int i = 0; i < 4; ++i)
    {
        sprintf_s(buffer, "[%.3f, %.3f, %.3f, %.3f]\n",
            matrix.m[i][0], matrix.m[i][1], matrix.m[i][2], matrix.m[i][3]);
        OutputDebugStringA(buffer);
    }
}


LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception)
{
    //時刻を取得して、時刻を名前に入れたファイルを作成。Dumpsディレクトリ以下に出力
    SYSTEMTIME time;
    GetLocalTime(&time);
    wchar_t filePath[MAX_PATH] = { 0 };
    CreateDirectory(L"./Dumps", nullptr);
    StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
    HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
    // processId (このexeのId) とクラッシュ (例外)の発生したthreadIdを取得
    DWORD processId = GetCurrentProcessId();
    DWORD threadId = GetCurrentThreadId();
    // 設定情報を入力
    MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
    minidumpInformation.ThreadId = threadId;
    minidumpInformation.ExceptionPointers = exception;
    minidumpInformation.ClientPointers = TRUE;
    // Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
    MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
    // 他に関連づけられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
    return EXCEPTION_EXECUTE_HANDLER;
}

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
    // ID3D12Resourceを格納するポインタ
    ID3D12Resource* pResource = nullptr;

    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

    // リソース記述子を作成
    D3D12_RESOURCE_DESC resourceDesc{};
    // バッファリソース。テクスチャの場合はまた別の設定をする
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = sizeInBytes; // Vector4を３頂点分
    // バッファの場合はこれらは１にする決まり
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    // バッファの場合はこれにする決まり
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // リソースを作成
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,        // ヒープのプロパティ
        D3D12_HEAP_FLAG_NONE,   // ヒープフラグ
        &resourceDesc,          // リソースの記述子
        D3D12_RESOURCE_STATE_GENERIC_READ,           // 初期状態
        nullptr,                // Clear値 (バッファの場合はnullptr)
        IID_PPV_ARGS(&pResource) // ID3D12Resourceポインタを取得
    );

    assert(SUCCEEDED(hr));

    return pResource; // 作成したリソースを返す
};

ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
    // ディスクリプタヒープの生成
    ID3D12DescriptorHeap* DescriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
    // レンダ―ターゲットビュー用
    DescriptorHeapDesc.Type = heapType;
    // ダブルバッファ用に２つ。多くたってかまわない。
    DescriptorHeapDesc.NumDescriptors = numDescriptors;
    // 
    DescriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    // エラーチェック
    HRESULT hr = device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&DescriptorHeap));
    // ディスクリプタヒープの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    return DescriptorHeap;
}

// DXCを使ってShaderをCompileする関数
IDxcBlob* CompileShader(
    // CompileするShaderファイルへのパス
    const std::wstring& filePath,
    // Compilerに仕様するProfile
    const wchar_t* profile,
    // 初期化で生成したものを３つ
    IDxcUtils* dxcUtils,
    IDxcCompiler3* dxcCompiler,
    IDxcIncludeHandler* includeHandler)
{
    ///////////////////////////////////////
    //// 1 hlslファイルを読む
    ///////////////////////////////////////
    // これからシェーダーをコンパイルする旨をログに出す
    Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}", filePath, profile)));
    // hlslファイルを読む
    IDxcBlobEncoding* shaderSource = nullptr;
    HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    // 読めなかったら停止する
    assert(SUCCEEDED(hr));
    // 読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer;
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8;

    ///////////////////////////////////////
    //// 2 Compileする
    ///////////////////////////////////////
    LPCWSTR arguments[] = {
        filePath.c_str(),			// コンパイル対象のhlslファイル名
        L"-E", L"main",				// エントリーポイントの指定。基本的にmain
        L"-T", profile,				// ShaderProfileの設定
        L"-Zi", L"-Qembed_debug",	// デバック用の情報を埋め込む
        L"-Od",						// 最適化を外しておく
        L"-Zpr",					// 目盛レイアウトは行優先
    };
    // 実際にシェーダーをコンパイルする
    IDxcResult* shaderResult = nullptr;
    hr = dxcCompiler->Compile(
        &shaderSourceBuffer,	// 読み込んだファイル
        arguments,				// コンパイルオプション
        _countof(arguments),	// コンパイルオプションの数
        includeHandler,			// includeが含まれた諸々
        IID_PPV_ARGS(&shaderResult)// コンパイル結果
    );
    // コンパイルエラーではなくdxcが起動出来ないなど致命的な状況
    assert(SUCCEEDED(hr));

    ///////////////////////////////////////
    //// 3 警告・エラーが出ていないか確認する
    ///////////////////////////////////////
    // 警告・エラーが出たらログにだして止める
    IDxcBlobUtf8* shaderError = nullptr;
    IDxcBlobUtf16* outputName = nullptr;
    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), &outputName);

    if (shaderError != nullptr && shaderError->GetStringLength() != 0)
    {
        Log(shaderError->GetStringPointer());
        assert(false); // コンパイルエラーが発生した場合は停止
    }

    ///////////////////////////////////////
    //// 4 Compile結果を受け取って返す
    ///////////////////////////////////////
    // コンパイル結果から実行用のバイナリ部分を取得
    IDxcBlob* shaderBlob = nullptr;
    hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    assert(SUCCEEDED(hr));
    // 成功したログを出す
    Log(ConvertString(std::format(L"Compile Succeeded. path:{}\n", filePath, profile)));
    // もう使わないリソースを解放
    shaderSource->Release();
    shaderResult->Release();
    // 実行用のバイナリを返却
    return shaderBlob;
}

// 1,Textureデータを読む
//DirectX::ScratchImage LoadTexture(const std::string& filePath)
//{
//    // テクスチャファイルを読んでプログラムを扱えるようにする
//    DirectX::ScratchImage image{};
//    std::wstring filePathw = ConvertString(filePath);
//    HRESULT hr = DirectX::LoadFromWICFile(filePathw.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
//    assert(SUCCEEDED(hr));
//
//    // ミップマップの作成
//    DirectX::ScratchImage mipImages{};
//    hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
//    assert(SUCCEEDED(hr));
//
//    // ミップマップ付きのデータを返す
//    return mipImages;
//}

// 2,
ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
    // 1,metadataを基にResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = UINT(metadata.width);
    resourceDesc.Height = UINT(metadata.height);
    resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
    resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or 配列Textureの配列数
    resourceDesc.Format = metadata.format; // TextureのFormat
    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。１固定
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使ってるのは２次元

    // 2,利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    // 3,Resourceを生成する
    ID3D12Resource* resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, // Heapの設定
        D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定
        &resourceDesc, // Resourceの設定
        D3D12_RESOURCE_STATE_COPY_DEST, // 初回のResourceState.Textureは基本読むだけ
        nullptr, // Clear最適解。使わないのでnullptr
        IID_PPV_ARGS(&resource) // 作成するResourceポインタへのポインタ
    );
    assert(SUCCEEDED(hr));

    return resource;
}

// 3,TextureResourceにデータを転送する
[[nodiscard]]
ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
    ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
    UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
    // Tetureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READ ResourceStateを変更する
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    commandList->ResourceBarrier(1, &barrier);
    return intermediateResource;
}

// DepthStencilTextureを作る
ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height)
{
    // 生成するResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;
    resourceDesc.Height = height;
    resourceDesc.MipLevels = 1; // mipmapの数
    resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // TextureのFormat
    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。１固定
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // ２次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使うよーとういう通知

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

    // 深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(１番遠い状態)でクリア
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceろあわせる

    // Resourceの生成
    ID3D12Resource* resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthClearValue,
        IID_PPV_ARGS(&resource)
    );
    assert(SUCCEEDED(hr));

    return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * index);
    return handleCPU;
}
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * index);
    return handleGPU;
}

// mtlファイルを読み込む関数
MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
    /////////////////
    // 変数宣言
    /////////////////
    MaterialData materialData;
    std::string line;

    /////////////////
    // ファイルを開く
    /////////////////
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());

    /////////////////
    // MaterialDataを構築する
    /////////////////
    while (std::getline(file, line))
    {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;

        // 
        if (identifier == "map_KD")
        {
            std::string textureFilename;
            s >> textureFilename;
            materialData.textureFilePath = directoryPath + "/" + textureFilename;
        }


    }

    /////////////////
    // 構築したMaterialDataをreturnする
    /////////////////
    return materialData;
}

// objファイルを読み込む関数
ModelData LoadOBJFile(const std::string& directoryPath, const std::string& filename)
{
    /////////////////
    // 変数宣言
    /////////////////
    ModelData modelData;
    std::vector<Vector4> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;
    std::string line;

    /////////////////
    // ファイルをひらく
    /////////////////
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());

    /////////////////
    // ModelDataを構築する
    /////////////////
    while (std::getline(file, line))
    {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;
        // 頂点位置
        if (identifier == "v")
        {
            Vector4 position;
            s >> position.x >> position.y >> position.z;
            position.x *= -1.0f;
            position.w = 1.0f;
            positions.push_back(position);
        }
        // 頂点テクスチャ座標
        else if (identifier == "vt")
        {
            Vector2 texcoord;
            s >> texcoord.x >> texcoord.y;
            texcoord.y = 1.0f - texcoord.y;
            texcoords.push_back(texcoord);
        }
        // 頂点法線
        else if (identifier == "vn")
        {
            Vector3 normal;
            s >> normal.x >> normal.y >> normal.z;
            normal.x *= -1.0f;
            normals.push_back(normal);
        }
        // 面
        else if (identifier == "f")
        {
            // 1行分の頂点定義をすべて取得
            std::vector<std::string> vertexDefs;
            std::string vertexDefinition;
            while (s >> vertexDefinition)
            {
                vertexDefs.push_back(vertexDefinition);
            }

            // 3頂点未満は無視
            if (vertexDefs.size() < 3) continue;

            // 四角形を三角形２つに五角形を三角形３つに変換
            for (size_t i = 1; i + 1 < vertexDefs.size(); ++i)
            {
                VertexData triangle[3];
                std::string vdefs[3] = { vertexDefs[0], vertexDefs[i], vertexDefs[i + 1] };
                for (int faceVertex = 0; faceVertex < 3; ++faceVertex)
                {
                    std::istringstream v(vdefs[faceVertex]);
                    uint32_t elementIndices[3] = {};
                    for (int element = 0; element < 3; ++element)
                    {
                        std::string index;
                        std::getline(v, index, '/');
                        elementIndices[element] = std::stoi(index);
                    }
                    Vector4 position = { 0,0,0,1 };
                    Vector2 texcoord = { 0,0 };
                    Vector3 normal = { 0,0,0 };
                    if (elementIndices[0] > 0 && elementIndices[0] <= positions.size())
                        position = positions[elementIndices[0] - 1];
                    if (elementIndices[1] > 0 && elementIndices[1] <= texcoords.size())
                        texcoord = texcoords[elementIndices[1] - 1];
                    if (elementIndices[2] > 0 && elementIndices[2] <= normals.size())
                        normal = normals[elementIndices[2] - 1];
                    triangle[faceVertex] = { position, texcoord, normal };
                }
                // 頂点の順序を逆にして追加（右手系→左手系変換のため）
                modelData.vertices.push_back(triangle[2]);
                modelData.vertices.push_back(triangle[1]);
                modelData.vertices.push_back(triangle[0]);
            }
        }

        // mtllib
        else if (identifier == "mtllib")
        {
            // materialTemplateLibraryファイルの名前を取得する
            std::string materialFilename;
            s >> materialFilename;
            // 基本的にmtlはobjファイルと同一階層に配置指せるので、ディレクトリ名とファイル名を渡す
            modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
        }
    }


    /////////////////
    // 構築したModelDataをreturnする
    /////////////////
    return modelData;
}