#include <cmath>
#include <cassert>
#include <Windows.h>
#include "definition.h"
#include "functions.h"


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
    return Vector3();
}

float Length(const Vector3& v)
{
    float Return{};
    float a{};

    a = sqrtf((v.x * v.x) + (v.y * v.y));
    Return = sqrtf((a * a) + (v.z * v.z));

    return Return;
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

void DrawSphere(VertexData* vertexData, uint32_t kSubdivision)
{
    if (kSubdivision == 0 || vertexData == nullptr) {
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
            const float nextLon = lon + kLonEvery;

            // テクスチャ座標を計算
            const float u = float(lonIndex) / float(kSubdivision);
            const float nextU = float(lonIndex + 1) / float(kSubdivision);
            const float v = 1.0f - float(latIndex) / float(kSubdivision);
            const float nextV = 1.0f - float(latIndex + 1) / float(kSubdivision);

            // 頂点データの開始インデックス
            const uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;


            // 頂点データを設定 (三角形1)
            vertexData[start + 0].position = { std::cos(lat) * std::cos(lon), std::sin(lat), std::cos(lat) * std::sin(lon), 1.0f };
            vertexData[start + 0].texcoord = { u, v };
            vertexData[start + 0].normal = { vertexData[start + 0].position.x,vertexData[start + 0].position.y,vertexData[start + 0].position.z };

            vertexData[start + 2].position = { std::cos(nextLat) * std::cos(nextLon), std::sin(nextLat), std::cos(nextLat) * std::sin(nextLon), 1.0f };
            vertexData[start + 2].texcoord = { nextU, nextV };
            vertexData[start + 2].normal = { vertexData[start + 2].position.x,vertexData[start + 2].position.y,vertexData[start + 2].position.z };

            vertexData[start + 1].position = { std::cos(nextLat) * std::cos(lon), std::sin(nextLat), std::cos(nextLat) * std::sin(lon), 1.0f };
            vertexData[start + 1].texcoord = { u, nextV };
            vertexData[start + 1].normal = { vertexData[start + 1].position.x,vertexData[start + 1].position.y,vertexData[start + 1].position.z };

            // 頂点データを設定 (三角形2)
            vertexData[start + 3].position = { std::cos(lat) * std::cos(lon), std::sin(lat), std::cos(lat) * std::sin(lon), 1.0f };
            vertexData[start + 3].texcoord = { u, v };
            vertexData[start + 3].normal = { vertexData[start + 3].position.x,vertexData[start + 3].position.y,vertexData[start + 3].position.z };

            vertexData[start + 5].position = { std::cos(lat) * std::cos(nextLon), std::sin(lat), std::cos(lat) * std::sin(nextLon), 1.0f };
            vertexData[start + 5].texcoord = { nextU, v };
            vertexData[start + 5].normal = { vertexData[start + 5].position.x,vertexData[start + 5].position.y,vertexData[start + 5].position.z };
            
            vertexData[start + 4].position = { std::cos(nextLat) * std::cos(nextLon), std::sin(nextLat), std::cos(nextLat) * std::sin(nextLon), 1.0f };
            vertexData[start + 4].texcoord = { nextU, nextV };
            vertexData[start + 4].normal = { vertexData[start + 4].position.x,vertexData[start + 4].position.y,vertexData[start + 4].position.z };
        }
    }
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
        assert(false); // コンパイルエラーが発生した場合は停止
    }

    ///////////////////////////////////////
    //// 4 Compile結果を受け取って返す
    ///////////////////////////////////////
    // コンパイル結果から実行用のバイナリ部分を取得
    IDxcBlob* shaderBlob = nullptr;
    hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    assert(SUCCEEDED(hr));
    // もう使わないリソースを解放
    shaderSource->Release();
    shaderResult->Release();
    // 実行用のバイナリを返却
    return shaderBlob;
}

//static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception)
//{
//    //時刻を取得して、時刻を名前に入れたファイルを作成。Dumpsディレクトリ以下に出力
//    SYSTEMTIME time;
//    GetLocalTime(&time);
//    wchar_t filePath[MAX_PATH] = { 0 };
//    CreateDirectory(L"./Dumps", nullptr);
//    StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
//    HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
//    // processId (このexeのId) とクラッシュ (例外)の発生したthreadIdを取得
//    DWORD processId = GetCurrentProcessId();
//    DWORD threadId = GetCurrentThreadId();
//    // 設定情報を入力
//    MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
//    minidumpInformation.ThreadId = threadId;
//    minidumpInformation.ExceptionPointers = exception;
//    minidumpInformation.ClientPointers = TRUE;
//    // Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
//    MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
//    // 他に関連づけられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
//    return EXCEPTION_EXECUTE_HANDLER;
//}
