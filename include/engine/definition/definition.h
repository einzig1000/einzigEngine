#pragma once
// 標準ライブラリ
#include <vector>
#include <string>

// Windows/DirectX
#include <initguid.h>
#include <dxgidebug.h>
#include <d3d12.h>
#include <dxgi1_6.h>

// ライブラリリンク
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d12.lib")

// WRL
#include <wrl.h>

// 外部ライブラリ
#include "externals/DirectXTex/DirectXTex.h"

#define WIDTH 1280
#define HEIGHT 720

template <typename T>
constexpr const T& my_min(const T& a, const T& b)
{
    return (a < b) ? a : b;
}

template <typename T>
constexpr T my_min(std::initializer_list<T> list)
{
    auto it = list.begin();
    T result = *it;
    ++it;
    for (; it != list.end(); ++it)
    {
        if (*it < result)
            result = *it;
    }
    return result;
}


template <typename T>
constexpr const T& my_max(const T& a, const T& b)
{
    return (a > b) ? a : b;
}

template <typename T>
constexpr T my_max(std::initializer_list<T> list)
{
    auto it = list.begin();
    T result = *it;
    ++it;
    for (; it != list.end(); ++it)
    {
        if (*it > result)
            result = *it;
    }
    return result;
}


enum class PHASE
{
    Phase_None,
    Phase_Title,
    Phase_GameScene,
    Phase_StageSelect,
    Phase_GameClear,
};

enum class LookAtMode
{
    None,
    Target,
    Front
};

enum class Anker
{
    Center,

    CenterLeft,
    CenterRight,
    CenterTop,
    CenterDown,

    LeftTop,
    RightTop,
    LeftDown,
    RightDown,
};



enum class Direction
{
    None = -1,
    Left = 0,
    Right = 1,
    Down = 2,
    Up = 3,
};

struct Vector2int
{
    int x = 0;
    int y = 0;
    Vector2int operator+(const Vector2int& rhs) const
    {
        return Vector2int{ x + rhs.x, y + rhs.y };
    }
    Vector2int operator-(const Vector2int& rhs) const
    {
        return Vector2int{ x - rhs.x, y - rhs.y };
    }
    bool operator==(const Vector2int& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Vector2int& rhs) const
    {
        return x != rhs.x || y != rhs.y;
    }
    bool operator<(const Vector2int& rhs) const
    {
        if (y != rhs.y)
        {
            return y < rhs.y;
        }
        return x < rhs.x;
    }
};

struct Vector2
{
    float x = 0;
    float y = 0;

    Vector2 operator+(const Vector2& rhs) const
    {
        return Vector2{ x + rhs.x, y + rhs.y };
    }
    Vector2 operator-(const Vector2& rhs) const
    {
        return Vector2{ x - rhs.x, y - rhs.y };
    }
    Vector2 operator*(float scalar) const
    {
        return Vector2{ x * scalar, y * scalar };
    }
    Vector2 operator/(float scalar) const
    {
        return Vector2{ x / scalar, y / scalar };
    }
    Vector2& operator+=(const Vector2& rhs)
    {
        x += rhs.x; y += rhs.y; return *this;
    }
    Vector2& operator-=(const Vector2& rhs)
    {
        x -= rhs.x; y -= rhs.y; return *this;
    }
    Vector2& operator*=(float scalar)
    {
        x *= scalar; y *= scalar; return *this;
    }
    Vector2& operator/=(float scalar)
    {
        x /= scalar; y /= scalar; return *this;
    }
    bool operator==(const Vector2& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Vector2& rhs) const
    {
        return !(*this == rhs);
    }

    // ベクトルの長さ 
    float Length() const
    {
        return std::sqrt(x * x + y * y);
    }
    // 平方根計算をしてない長澤
    float LengthSq() const
    {
        return x * x + y * y;
    }
    // ベクトルを正規化　※　自身を変更する　※
    Vector2& Normalize()
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
    // 正規化された新しいベクトルを返す　※　自身は変更されない　※
    Vector2 Normalized() const
    {
        Vector2 result = *this;
        result.Normalize();
        return result;
    }
    // 内積
    float Dot(const Vector2& rhs) const
    {
        return x * rhs.x + y * rhs.y;
    }
};

struct Vector3
{
    float x = 0;
    float y = 0;
    float z = 0;

    Vector3 operator-() const
    {
        return Vector3(-x, -y, -z);
    }
    Vector3 operator+(const Vector3& rhs) const
    {
        return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    Vector3 operator-(const Vector3& rhs) const
    {
        return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
    }
    Vector3 operator*(float scalar) const
    {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    Vector3 operator/(float scalar) const
    {
        // ゼロ除算しようとした者を殺す
        if (std::abs(scalar) < 0.00001f)
        {
            return Vector3(0.0f, 0.0f, 0.0f);
        }
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    Vector3& operator+=(const Vector3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    Vector3& operator-=(const Vector3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }
    Vector3& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    Vector3& operator/=(float scalar)
    {
        // ゼロ除算しようとした者を殺す
        if (std::abs(scalar) < 0.00001f)
        {
            return *this;
        }
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    bool operator==(const Vector3& rhs) const
    {
        // 多少のズレは許容。
        const float EPSILON = 0.00001f;
        return std::abs(x - rhs.x) < EPSILON &&
            std::abs(y - rhs.y) < EPSILON &&
            std::abs(z - rhs.z) < EPSILON;
    }
    bool operator!=(const Vector3& rhs) const
    {
        return !(*this == rhs);
    }

    // ベクトルの長さ
    float Length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }
    // 平方根計算をしてない長澤
    float LengthSq() const
    {
        return x * x + y * y + z * z;
    }
    // ベクトルを正規化　※　自身を変更する　※
    Vector3& Normalize()
    {
        float len = Length();
        if (std::abs(len) > 0.00001f)
        { // ゼロ除算は殺すって言わなかった？
            x /= len;
            y /= len;
            z /= len;
        }
        return *this;
    }
    // 正規化された新しいベクトルを返す　※　自身は変更されない　※
    Vector3 Normalized() const
    {
        Vector3 result = *this;
        result.Normalize();
        return result;
    }
    // 内積
    float Dot(const Vector3& rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }
    // 外積
    Vector3 Cross(const Vector3& rhs) const
    {
        return Vector3(
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        );
    }
    // 反射角
    Vector3 Reflect(const Vector3& input, const Vector3& normal)
    {
        Vector3 result;

        result = input - ((normal * (input.Dot(normal))) * 2);

        return result;
    }
};

struct Vector4
{
    float x = 0, y = 0, z = 0, w = 0;

    Vector4 operator+(const Vector4& rhs) const
    {
        return Vector4{ x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
    }
    Vector4 operator-(const Vector4& rhs) const
    {
        return Vector4{ x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
    }
    Vector4 operator*(float scalar) const
    {
        return Vector4{ x * scalar, y * scalar, z * scalar, w * scalar };
    }
    Vector4 operator/(float scalar) const
    {
        return Vector4{ x / scalar, y / scalar, z / scalar, w / scalar };
    }
    Vector4& operator+=(const Vector4& rhs)
    {
        x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;
    }
    Vector4& operator-=(const Vector4& rhs)
    {
        x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;
    }
    Vector4& operator*=(float scalar)
    {
        x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this;
    }
    Vector4& operator/=(float scalar)
    {
        x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this;
    }
    bool operator==(const Vector4& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
    bool operator!=(const Vector4& rhs) const
    {
        return !(*this == rhs);
    }

    // ベクトルの長さ
    float Length() const
    {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }
    // 平方根計算をしてない長澤
    float LengthSq() const
    {
        return x * x + y * y + z * z + w * w;
    }
    // ベクトルを正規化　※　自身を変更する　※
    Vector4& Normalize()
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
    // 正規化された新しいベクトルを返す　※　自身は変更されない　※
    Vector4 Normalized() const
    {
        Vector4 result = *this;
        result.Normalize();
        return result;
    }
    // 内積
    float Dot(const Vector4& rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
    }
};

struct Matrix3x3
{
    float m[3][3] = {
    { 0,0,0 },
    { 0,0,0 },
    { 0,0,0 } };

    Matrix3x3 operator+(const Matrix3x3& rhs) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.m[i][j] = m[i][j] + rhs.m[i][j];
            }
        }
        return result;
    }
    Matrix3x3 operator-(const Matrix3x3& rhs) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.m[i][j] = m[i][j] - rhs.m[i][j];
            }
        }
        return result;
    }
    Matrix3x3 operator*(float scalar) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.m[i][j] = m[i][j] * scalar;
            }
        }
        return result;
    }
    Matrix3x3 operator*(const Matrix3x3& rhs) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 3; ++k)
                {
                    result.m[i][j] += m[i][k] * rhs.m[k][j];
                }
            }
        }
        return result;
    }
    bool operator==(const Matrix3x3& rhs) const
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                if (m[i][j] != rhs.m[i][j]) return false;
            }
        }
        return true;
    }
    bool operator!=(const Matrix3x3& rhs) const
    {
        return !(*this == rhs);
    }

};

struct Matrix4x4
{
    float m[4][4] = {
    { 1,0,0,0 },
    { 0,1,0,0 },
    { 0,0,1,0 },
    { 0,0,0,1 } };

    Matrix4x4 operator+(const Matrix4x4& rhs) const
    {
        Matrix4x4 result;
        for (int j = 0; j < 4; ++j)
        {
            for (int i = 0; i < 4; ++i)
            {
                result.m[i][j] = m[i][j] + rhs.m[i][j];
            }
        }
        return result;
    }
    Matrix4x4 operator-(const Matrix4x4& rhs) const
    {
        Matrix4x4 result;
        for (int j = 0; j < 4; ++j)
        {
            for (int i = 0; i < 4; ++i)
            {
                result.m[i][j] = m[i][j] - rhs.m[i][j];
            }
        }
        return result;
    }
    Matrix4x4 operator*(float scalar) const
    {
        Matrix4x4 result;
        for (int j = 0; j < 4; ++j)
        {
            for (int i = 0; i < 4; ++i)
            {
                result.m[i][j] = m[i][j] * scalar;
            }
        }
        return result;
    }
    Matrix4x4 operator*(const Matrix4x4& rhs) const
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k)
                {
                    result.m[i][j] += m[i][k] * rhs.m[k][j];
                }
            }
        }
        return result;
    }

    bool operator==(const Matrix4x4& rhs) const
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (m[i][j] != rhs.m[i][j]) return false;
        return true;
    }
    bool operator!=(const Matrix4x4& rhs) const
    {
        return !(*this == rhs);
    }

    // 逆行列
    Matrix4x4 Inverse() const
    {
        Matrix4x4 Return{};

        float A = (m[0][0] * m[1][1] * m[2][2] * m[3][3]) + (m[0][0] * m[1][2] * m[2][3] * m[3][1]) + (m[0][0] * m[1][3] * m[2][1] * m[3][2])
            - (m[0][0] * m[1][3] * m[2][2] * m[3][1]) - (m[0][0] * m[1][2] * m[2][1] * m[3][3]) - (m[0][0] * m[1][1] * m[2][3] * m[3][2])
            - (m[0][1] * m[1][0] * m[2][2] * m[3][3]) - (m[0][2] * m[1][0] * m[2][3] * m[3][1]) - (m[0][3] * m[1][0] * m[2][1] * m[3][2])
            + (m[0][3] * m[1][0] * m[2][2] * m[3][1]) + (m[0][2] * m[1][0] * m[2][1] * m[3][3]) + (m[0][1] * m[1][0] * m[2][3] * m[3][2])
            + (m[0][1] * m[1][2] * m[2][0] * m[3][3]) + (m[0][2] * m[1][3] * m[2][0] * m[3][1]) + (m[0][3] * m[1][1] * m[2][0] * m[3][2])
            - (m[0][3] * m[1][2] * m[2][0] * m[3][1]) - (m[0][2] * m[1][1] * m[2][0] * m[3][3]) - (m[0][1] * m[1][3] * m[2][0] * m[3][2])
            - (m[0][1] * m[1][2] * m[2][3] * m[3][0]) - (m[0][2] * m[1][3] * m[2][1] * m[3][0]) - (m[0][3] * m[1][1] * m[2][2] * m[3][0])
            + (m[0][3] * m[1][2] * m[2][1] * m[3][0]) + (m[0][2] * m[1][1] * m[2][3] * m[3][0]) + (m[0][1] * m[1][3] * m[2][2] * m[3][0]);

        Return.m[0][0] = (1 / A) * ((m[1][1] * m[2][2] * m[3][3]) + (m[1][2] * m[2][3] * m[3][1]) + (m[1][3] * m[2][1] * m[3][2]) - (m[1][3] * m[2][2] * m[3][1]) - (m[1][2] * m[2][1] * m[3][3]) - (m[1][1] * m[2][3] * m[3][2]));
        Return.m[0][1] = (1 / A) * ((m[0][3] * m[2][2] * m[3][1]) + (m[0][2] * m[2][1] * m[3][3]) + (m[0][1] * m[2][3] * m[3][2]) - (m[0][1] * m[2][2] * m[3][3]) - (m[0][2] * m[2][3] * m[3][1]) - (m[0][3] * m[2][1] * m[3][2]));
        Return.m[0][2] = (1 / A) * ((m[0][1] * m[1][2] * m[3][3]) + (m[0][2] * m[1][3] * m[3][1]) + (m[0][3] * m[1][1] * m[3][2]) - (m[0][3] * m[1][2] * m[3][1]) - (m[0][2] * m[1][1] * m[3][3]) - (m[0][1] * m[1][3] * m[3][2]));
        Return.m[0][3] = (1 / A) * ((m[0][3] * m[1][2] * m[2][1]) + (m[0][2] * m[1][1] * m[2][3]) + (m[0][1] * m[1][3] * m[2][2]) - (m[0][1] * m[1][2] * m[2][3]) - (m[0][2] * m[1][3] * m[2][1]) - (m[0][3] * m[1][1] * m[2][2]));

        Return.m[1][0] = (1 / A) * ((m[1][3] * m[2][2] * m[3][0]) + (m[1][2] * m[2][0] * m[3][3]) + (m[1][0] * m[2][3] * m[3][2]) - (m[1][0] * m[2][2] * m[3][3]) - (m[1][2] * m[2][3] * m[3][0]) - (m[1][3] * m[2][0] * m[3][2]));
        Return.m[1][1] = (1 / A) * ((m[0][0] * m[2][2] * m[3][3]) + (m[0][2] * m[2][3] * m[3][0]) + (m[0][3] * m[2][0] * m[3][2]) - (m[0][3] * m[2][2] * m[3][0]) - (m[0][2] * m[2][0] * m[3][3]) - (m[0][0] * m[2][3] * m[3][2]));
        Return.m[1][2] = (1 / A) * ((m[0][3] * m[1][2] * m[3][0]) + (m[0][2] * m[1][0] * m[3][3]) + (m[0][0] * m[1][3] * m[3][2]) - (m[0][0] * m[1][2] * m[3][3]) - (m[0][2] * m[1][3] * m[3][0]) - (m[0][3] * m[1][0] * m[3][2]));
        Return.m[1][3] = (1 / A) * ((m[0][0] * m[1][2] * m[2][3]) + (m[0][2] * m[1][3] * m[2][0]) + (m[0][3] * m[1][0] * m[2][2]) - (m[0][3] * m[1][2] * m[2][0]) - (m[0][2] * m[1][0] * m[2][3]) - (m[0][0] * m[1][3] * m[2][2]));

        Return.m[2][0] = (1 / A) * ((m[1][0] * m[2][1] * m[3][3]) + (m[1][1] * m[2][3] * m[3][0]) + (m[1][3] * m[2][0] * m[3][1]) - (m[1][3] * m[2][1] * m[3][0]) - (m[1][1] * m[2][0] * m[3][3]) - (m[1][0] * m[2][3] * m[3][1]));
        Return.m[2][1] = (1 / A) * ((m[0][3] * m[2][1] * m[3][0]) + (m[0][1] * m[2][0] * m[3][3]) + (m[0][0] * m[2][3] * m[3][1]) - (m[0][0] * m[2][1] * m[3][3]) - (m[0][1] * m[2][3] * m[3][0]) - (m[0][3] * m[2][0] * m[3][1]));
        Return.m[2][2] = (1 / A) * ((m[0][0] * m[1][1] * m[3][3]) + (m[0][1] * m[1][3] * m[3][0]) + (m[0][3] * m[1][0] * m[3][1]) - (m[0][3] * m[1][1] * m[3][0]) - (m[0][1] * m[1][0] * m[3][3]) - (m[0][0] * m[1][3] * m[3][1]));
        Return.m[2][3] = (1 / A) * ((m[0][3] * m[1][1] * m[2][0]) + (m[0][1] * m[1][0] * m[2][3]) + (m[0][0] * m[1][3] * m[2][1]) - (m[0][0] * m[1][1] * m[2][3]) - (m[0][1] * m[1][3] * m[2][0]) - (m[0][3] * m[1][0] * m[2][1]));

        Return.m[3][0] = (1 / A) * ((m[1][2] * m[2][1] * m[3][0]) + (m[1][1] * m[2][0] * m[3][2]) + (m[1][0] * m[2][2] * m[3][1]) - (m[1][0] * m[2][1] * m[3][2]) - (m[1][1] * m[2][2] * m[3][0]) - (m[1][2] * m[2][0] * m[3][1]));
        Return.m[3][1] = (1 / A) * ((m[0][0] * m[2][1] * m[3][2]) + (m[0][1] * m[2][2] * m[3][0]) + (m[0][2] * m[2][0] * m[3][1]) - (m[0][2] * m[2][1] * m[3][0]) - (m[0][1] * m[2][0] * m[3][2]) - (m[0][0] * m[2][2] * m[3][1]));
        Return.m[3][2] = (1 / A) * ((m[0][2] * m[1][1] * m[3][0]) + (m[0][1] * m[1][0] * m[3][2]) + (m[0][0] * m[1][2] * m[3][1]) - (m[0][0] * m[1][1] * m[3][2]) - (m[0][1] * m[1][2] * m[3][0]) - (m[0][2] * m[1][0] * m[3][1]));
        Return.m[3][3] = (1 / A) * ((m[0][0] * m[1][1] * m[2][2]) + (m[0][1] * m[1][2] * m[2][0]) + (m[0][2] * m[1][0] * m[2][1]) - (m[0][2] * m[1][1] * m[2][0]) - (m[0][1] * m[1][0] * m[2][2]) - (m[0][0] * m[1][2] * m[2][1]));

        return Return;
    }
    // 転置行列
    Matrix4x4 Transpose() const
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
    // 単位行列
    Matrix4x4 MakeIdentity()
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }
    static Matrix4x4 MakeIdentity4x4()
    {
        Matrix4x4 Return{};

        for (int i = 0; i < 4; ++i)
        {
            Return.m[i][i] = 1.0f;
        }

        return Return;
    }
    // 平行移動行列
    static Matrix4x4 MakeTranslateMatrix(const Vector3& translate)
    {
        Matrix4x4 result = MakeIdentity4x4();
        result.m[3][0] = translate.x;
        result.m[3][1] = translate.y;
        result.m[3][2] = translate.z;
        return result;
    }
    // 拡縮行列
    static Matrix4x4 MakeScaleMatrix(const Vector3& scale)
    {
        Matrix4x4 result = MakeIdentity4x4();
        result.m[0][0] = scale.x;
        result.m[1][1] = scale.y;
        result.m[2][2] = scale.z;

        return result;
    }
    // Ｘ軸回転行列
    static Matrix4x4 MakeRotateXMatrix(float radian)
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
    static Matrix4x4 MakeRotateYMatrix(float radian)
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
    static Matrix4x4 MakeRotateZMatrix(float radian)
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
    static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate)
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
    // 透視投影行列
    static Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
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
    // 正射影行列(平行投影行列)
    static Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
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
    static Matrix4x4 MakeViewPortMatrix(float left, float top, float width, float height, float minD, float maxD)
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
};


struct CollisionInf
{
	Vector2int pair;  // 衝突したオブジェクトのAABBの番号ペア
    Vector3 depth;    // 浸入深度
};

struct Sphere
{
    Vector3 center;
    float radius = 1.0f;
};

struct SphereXYZ
{
    Vector3 center;
    Vector3 radius = { 1.0f, 1.0f, 1.0f };
};

struct Plane
{
    Vector3 normal; // 法線
    float distance = 0.0f;
};

struct Triangle
{
    Vector3 vertices[3];
};

struct Vertex
{
    Vector2 LT;
    Vector2 RT;
    Vector2 LB;
    Vector2 RB;
};

struct Transforms
{
    Vector3 scale = { 1,1,1 };
    Vector3 rotate = { 0,0,0 };
    Vector3 translate = { 0,0,0 };
    Matrix4x4 World;
    Matrix4x4* parentWorld = nullptr;
};

struct VertexData
{
    Vector4 position;
    Vector2 texcoord;
    Vector3 normal;
};

struct Line
{
    // 始点
    Vector3 origin;
    // 終点ベクトル
    Vector3 diff;
};

struct Ray
{
    // 始点
    Vector3 origin;
    // 終点ベクトル
    Vector3 diff;
};

struct Segment
{
    // 始点
    Vector3 origin;
    // 終点ベクトル
    Vector3 diff;
};

struct Material
{
    Vector4 color;
    int32_t enableLighting;
    float padding[3];
    Matrix4x4 uvTransform;
};

struct TransformationMatrix
{
    Matrix4x4 WVP;
    Matrix4x4 World;
};

struct DirectionalLight
{
    Vector4 color;
    Vector3 direction;
    float intensity;//輝度
    int mode;
};

struct MaterialData
{
    std::string textureFilePath;
};

struct ModelData
{
    std::vector<VertexData> vertices;
    MaterialData material;
};


struct AABB
{
    Vector3 min;
    Vector3 max;

    Vector3 center()const
    {
        return Vector3{
            (min.x + max.x) / 2.0f,
            (min.y + max.y) / 2.0f,
            (min.z + max.z) / 2.0f,
        };
    };

    Vector3 GetCollisionDepth(const AABB& other)const
    {
        Vector3 depth;
        depth.x = my_min(max.x, other.max.x) - my_max(min.x, other.min.x);
        depth.y = my_min(max.y, other.max.y) - my_max(min.y, other.min.y);
        depth.z = my_min(max.z, other.max.z) - my_max(min.z, other.min.z);
        if (depth.x <= 0.0f || depth.y <= 0.0f || depth.z <= 0.0f)
        {
            return { 0.0f, 0.0f, 0.0f };
        }
        return depth;
    }
};

struct Object3D
{
    // モデルデータ
    ModelData modelData;

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    UINT vertexBufferSize = 0;

    // 変換行列
    Transforms transform;

    // AABB
    std::vector<AABB> aabb;

    // 識別ナンバー
    uint32_t number = 0;
};

struct TextureData
{
    DirectX::TexMetadata metadata;
    DirectX::ScratchImage mipImage;
    uint32_t number;
    Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
};

struct DrawData
{
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    const TextureData* texture;
};

struct D3DResourceLeakChecker
{
    ~D3DResourceLeakChecker()
    {
        // リソースリーク確認
        Microsoft::WRL::ComPtr <IDXGIDebug1> debug;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
        {
            debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
            debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
            debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
        }
    }
};

struct mouseButtenState
{
    bool leftButton;
    bool rightButton;
    bool middleButton;
};


enum class BlendMode
{
    // ブレンドなし
    kBlendModeNone,
    // 通常アルファブレンド
    kBlendModeNormal,
    // 加算
    kBlendModeAdd,
    // 減算
    kBlendModeSub,
    // 乗算
    kBlendModeMul,
    // スクリーン
    kBlendModeScreen,
    // 使用したら殺す
    Wireframe
};

struct DrawOptions
{
    // ワイヤーフレーム
    bool wireframe = false;
    // ライティングするか
    bool enableLighting = true;
    // ブレンドモード
    BlendMode blendMode = BlendMode::kBlendModeNormal;
};

struct DrawParticleOptions
{
    // エミッターはAABB型か球型か
    // true = AABB　false = 球
    bool emitterShape = true;
    // 全パーティクルがtarget方向に向かうかエミッターとtargetの垂直方向に向かうか
    // trueなら垂直方向、falseならtarget方向
    bool targetDirection = true;
    // エミッター内部でも発生するか外殻上でのみ発生するか
    // trueなら内部でも発生、falseなら外殻のみ
    bool spawnInsideEmitter = true;
    // ビルボードか否か
    bool toCamera = false;
};


enum class EaseType
{
    LINEAR,
    IN_SINE,
    OUT_SINE,
    IN_OUT_SINE,
    IN_QUAD,
    OUT_QUAD,
    IN_OUT_QUAD,
    IN_CUBIC,
    OUT_CUBIC,
    IN_OUT_CUBIC,
    IN_QUART,
    OUT_QUART,
    IN_OUT_QUART,
    IN_QUINT,
    OUT_QUINT,
    IN_OUT_QUINT,
    IN_EXPO,
    OUT_EXPO,
    IN_OUT_EXPO,
    IN_CIRC,
    OUT_CIRC,
    IN_OUT_CIRC,
    IN_BACK,
    OUT_BACK,
    IN_OUT_BACK,
    IN_ELASTIC,
    OUT_ELASTIC,
    IN_OUT_ELASTIC,
    IN_BOUNCE,
    OUT_BOUNCE,
};

struct ParticleInf
{
    Vector3 velocity;
    int liveTime = 0;
};

//enum class DestructionType
//{
//    // 透明になっていく
//    FadeOut,
//    // 小さくなっていく
//    ToSmall,
//};
