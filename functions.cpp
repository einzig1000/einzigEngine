#include <cmath>
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

    Matrix4x4 scaleMatrix{};
    Matrix4x4 rotateXMatrix;
    Matrix4x4 rotateYMatrix;
    Matrix4x4 rotateZMatrix;
    Matrix4x4 rotateXYZMatrix{};
    Matrix4x4 translateMatrix{};
    Matrix4x4 resultMatrix{};

    // 拡縮行列
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            scaleMatrix.m[i][j] = 0.0f;
        }
    }
    scaleMatrix.m[0][0] = scale.x;
    scaleMatrix.m[1][1] = scale.y;
    scaleMatrix.m[2][2] = scale.z;
    scaleMatrix.m[3][3] = 1.0f;

    // 回転行列
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            rotateXYZMatrix.m[i][j] = 0.0f;
        }
    }
    rotateXMatrix = MakeRotateXMatrix(rotate.x);
    rotateYMatrix = MakeRotateYMatrix(rotate.y);
    rotateZMatrix = MakeRotateZMatrix(rotate.z);
    rotateXYZMatrix = Mul(rotateXMatrix, Mul(rotateYMatrix, rotateZMatrix));

    // 移動行列
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            translateMatrix.m[i][j] = 0.0f;
        }
    }
    translateMatrix.m[0][0] = 1.0f;
    translateMatrix.m[1][1] = 1.0f;
    translateMatrix.m[2][2] = 1.0f;
    translateMatrix.m[3][3] = 1.0f;
    translateMatrix.m[3][0] = translate.x;
    translateMatrix.m[3][1] = translate.y;
    translateMatrix.m[3][2] = translate.z;

    // 行列の結合
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            resultMatrix.m[i][j] = 0.0f;
        }
    }

    resultMatrix.m[0][0] = scaleMatrix.m[0][0] * rotateXYZMatrix.m[0][0];
    resultMatrix.m[0][1] = scaleMatrix.m[0][0] * rotateXYZMatrix.m[0][1];
    resultMatrix.m[0][2] = scaleMatrix.m[0][0] * rotateXYZMatrix.m[0][2];
    resultMatrix.m[0][3] = 0;
    resultMatrix.m[1][0] = scaleMatrix.m[1][1] * rotateXYZMatrix.m[1][0];
    resultMatrix.m[1][1] = scaleMatrix.m[1][1] * rotateXYZMatrix.m[1][1];
    resultMatrix.m[1][2] = scaleMatrix.m[1][1] * rotateXYZMatrix.m[1][2];
    resultMatrix.m[1][3] = 0;
    resultMatrix.m[2][0] = scaleMatrix.m[2][2] * rotateXYZMatrix.m[2][0];
    resultMatrix.m[2][1] = scaleMatrix.m[2][2] * rotateXYZMatrix.m[2][1];
    resultMatrix.m[2][2] = scaleMatrix.m[2][2] * rotateXYZMatrix.m[2][2];
    resultMatrix.m[2][3] = 0;
    resultMatrix.m[3][0] = translateMatrix.m[3][0];
    resultMatrix.m[3][1] = translateMatrix.m[3][1];
    resultMatrix.m[3][2] = translateMatrix.m[3][2];
    resultMatrix.m[3][3] = 1;


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

// 正射影行列
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
