#include <cmath>
#ifndef M_PI  
#define M_PI 3.14159265358979323846  
#endif
#include <cassert>
#include "definition.h"

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

float Dot(const Vector3& v1, const Vector3& v2)
{
    float Return{};

    Return = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);

    return Return;
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
    Matrix4x4 rotateXYZMatrix = Mul(rotateXMatrix, Mul(rotateYMatrix, rotateZMatrix));
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


//void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix)
//{
//    // グリッドの半分の幅
//    const float kGridHalfWidth = 2.0f;
//    // 分割数(偶数にするのが好ましい)
//    const uint32_t kSubdivision = 10;
//    // １つ分の長さ
//    const float kGridEvery = ((kGridHalfWidth * 2.0f) / float(kSubdivision));
//
//    // 奥から手前への線を順に引いていく
//    for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex)
//    {
//        // ワールド座標系上の始点と終点を求める
//        float x = -kGridHalfWidth + xIndex * kGridEvery;
//
//        Vector3 startWorldX = { x, 0.0f, -kGridHalfWidth };
//        Vector3 endWorldX = { x, 0.0f, kGridHalfWidth };
//
//        Vector3 startWorldZ = { -kGridHalfWidth, 0.0f, x };
//        Vector3 endWorldZ = { kGridHalfWidth, 0.0f, x };
//
//        // スクリーン座標系まで変換する
//        Vector3 startScreenX = Transform(Transform(startWorldX, viewProjectionMatrix), viewportMatrix);
//        Vector3 endScreenX = Transform(Transform(endWorldX, viewProjectionMatrix), viewportMatrix);
//
//        Vector3 startScreenZ = Transform(Transform(startWorldZ, viewProjectionMatrix), viewportMatrix);
//        Vector3 endScreenZ = Transform(Transform(endWorldZ, viewProjectionMatrix), viewportMatrix);
//
//        // 変換した座標を用いて表示する
//        Novice::DrawLine(
//            static_cast<int>(startScreenX.x), static_cast<int>(startScreenX.y),
//            static_cast<int>(endScreenX.x), static_cast<int>(endScreenX.y),
//            0xAAAAAAFF
//        );
//
//        Novice::DrawLine(
//            static_cast<int>(startScreenZ.x), static_cast<int>(startScreenZ.y),
//            static_cast<int>(endScreenZ.x), static_cast<int>(endScreenZ.y),
//            0xAAAAAAFF
//        );
//        if (xIndex == kSubdivision / 2)
//        {
//            Novice::DrawLine(
//                static_cast<int>(startScreenX.x), static_cast<int>(startScreenX.y),
//                static_cast<int>(endScreenX.x), static_cast<int>(endScreenX.y),
//                0x000000FF
//            );
//
//            Novice::DrawLine(
//                static_cast<int>(startScreenZ.x), static_cast<int>(startScreenZ.y),
//                static_cast<int>(endScreenZ.x), static_cast<int>(endScreenZ.y),
//                0x000000FF
//            );
//        }
//    }
//}

//void DrawSphere(VertexData* vertexData, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color)
void DrawSphere(VertexData* vertexData)
{
    // 分割数
    const uint32_t kSubdivision = 16;
    // 経度分割１つ分の角度
    const float kLonEvery = float((2 * M_PI) / kSubdivision);
    // 緯度分割１つ分の角度
    const float kLatEvery = float(M_PI / kSubdivision);

    // 緯度の方向に分割 -π/2 ～ π/2
    for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
    {
        // 現在の緯度
        float lat = float(-M_PI / 2.0f + latIndex * kLatEvery);
        float nextLat = lat + kLatEvery;

        // 経度方向に分割 0 ～ 2π
        for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
        {
            // 現在の経度
            float lon = lonIndex * kLonEvery;
            float nextLon = lon + kLonEvery;

            float u = float(lonIndex) / float(kSubdivision);
            float v = 1.0f - float(latIndex) / float(kSubdivision);

            uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;

            vertexData[start].position.x = std::cos(lat) * std::cos(lon);
            vertexData[start].position.y = std::sin(lat);
            vertexData[start].position.z = std::cos(lat) * std::sin(lon);
            vertexData[start].position.w = 0.0f;
            vertexData[start].texcoord.x = u;
            vertexData[start].texcoord.y = v;

            vertexData[start + 1].position.x = std::cos(nextLat) * std::cos(nextLon);
            vertexData[start + 1].position.y = std::sin(nextLat);
            vertexData[start + 1].position.z = std::cos(nextLat) * std::sin(nextLon);
            vertexData[start + 1].position.w = 1.0f;
            vertexData[start + 1].texcoord.x = u;
            vertexData[start + 1].texcoord.y = v;

            vertexData[start + 2].position.x = std::cos(nextLat) * std::cos(lon);
            vertexData[start + 2].position.y = std::sin(nextLat);
            vertexData[start + 2].position.z = std::cos(nextLat) * std::sin(lon);
            vertexData[start + 2].position.w = 1.0f;
            vertexData[start + 2].texcoord.x = u;
            vertexData[start + 2].texcoord.y = v;




            vertexData[start + 3].position.x = std::cos(lat) * std::cos(lon);
            vertexData[start + 3].position.y = std::sin(lat);
            vertexData[start + 3].position.z = std::cos(lat) * std::sin(lon);
            vertexData[start + 3].position.w = 1.0f;
            vertexData[start + 3].texcoord.x = u;
            vertexData[start + 3].texcoord.y = v;

            vertexData[start + 4].position.x = std::cos(lat) * std::cos(nextLon);
            vertexData[start + 4].position.y = std::sin(lat);
            vertexData[start + 4].position.z = std::cos(lat) * std::sin(nextLon);
            vertexData[start + 4].position.w = 1.0f;
            vertexData[start + 4].texcoord.x = u;
            vertexData[start + 4].texcoord.y = v;

            vertexData[start + 5].position.x = std::cos(nextLat) * std::cos(nextLon);
            vertexData[start + 5].position.y = std::sin(nextLat);
            vertexData[start + 5].position.z = std::cos(nextLat) * std::sin(nextLon);
            vertexData[start + 5].position.w = 1.0f;
            vertexData[start + 5].texcoord.x = u;
            vertexData[start + 5].texcoord.y = v;




            // A, B, C, Dをスクリーン座標系まで変換
            //Vector3 screenA = Transform(Transform(A, viewProjectionMatrix), viewportMatrix);
            //Vector3 screenB = Transform(Transform(B, viewProjectionMatrix), viewportMatrix);
            //Vector3 screenC = Transform(Transform(C, viewProjectionMatrix), viewportMatrix);
            //Vector3 screenD = Transform(Transform(D, viewProjectionMatrix), viewportMatrix);
        }
    }
}

