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
#define eps 1e-6f

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
    Phase_Test,
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
    float Length() const;
    // 平方根計算をしてない長澤
    float LengthSq() const;
    // ベクトルを正規化　※　自身を変更する　※
    Vector2& Normalize();
    // 正規化された新しいベクトルを返す　※　自身は変更されない　※
    Vector2 Normalized() const;
    // 内積
    float Dot(const Vector2& rhs) const;
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
    float Length() const;
    // 平方根計算をしてない長澤
    float LengthSq() const;
    // ベクトルを正規化　※　自身を変更する　※
    Vector3& Normalize();
    // 正規化された新しいベクトルを返す　※　自身は変更されない　※
    Vector3 Normalized() const;
    // 内積
    float Dot(const Vector3& rhs) const;
    // 外積
    Vector3 Cross(const Vector3& rhs) const;
    // 反射角
    Vector3 Reflect(const Vector3& input, const Vector3& normal);
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
    float Length() const;
    // 平方根計算をしてない長澤
    float LengthSq() const;
    // ベクトルを正規化　※　自身を変更する　※
    Vector4& Normalize();
    // 正規化された新しいベクトルを返す　※　自身は変更されない　※
    Vector4 Normalized() const;
    // 内積
    float Dot(const Vector4& rhs) const;
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
    Matrix4x4 Inverse() const;
    // 転置行列
    Matrix4x4 Transpose() const;
    // 単位行列
    static Matrix4x4 MakeIdentity4x4();
    // 平行移動行列
    static Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
    // 拡縮行列
    static Matrix4x4 MakeScaleMatrix(const Vector3& scale);
    // Ｘ軸回転行列
    static Matrix4x4 MakeRotateXMatrix(float radian);
    // Ｙ軸回転行列
    static Matrix4x4 MakeRotateYMatrix(float radian);
    // Ｚ軸回転行列
    static Matrix4x4 MakeRotateZMatrix(float radian);
    // ３次元アフィン変換行列
    static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
    // 透視投影行列
    static Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
    // 正射影行列(平行投影行列)
    static Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
    // ビューポート変換
    static Matrix4x4 MakeViewPortMatrix(float left, float top, float width, float height, float minD, float maxD);
};

struct CollisionFlags
{
    enum
    {
        NONE = 0x00000000,
        FRONT = 0x00000001,
        BACK = 0x00000002,
        LEFT = 0x00000004,
        RIGHT = 0x00000008,
        TOP = 0x00000010,
        BOTTOM = 0x00000020,
        INSIDE = 0x00000040,
	};
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

enum class CollisionResult
{
    非衝突,
    接触,
    衝突
};

struct AABB
{
    Vector3 min;
    Vector3 max;

    Vector3 center()const;

    Vector3 GetCollisionDepth(const AABB& other)const;
};

struct VertexData
{
    Vector4 position;
    Vector2 texcoord;
    Vector3 normal;
};

struct ModelData
{
    std::vector<VertexData> vertices;
    MaterialData material;
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

	// ファイルパス
    std::string filePath;
};

struct TextureData
{
    DirectX::TexMetadata metadata;
    DirectX::ScratchImage mipImage;
    uint32_t number;
	std::string filePath;
    Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
};


struct KeyState
{
    bool curr = false;           // 今フレームの押下状態
    bool prev = false;           // 前フレームの押下状態
    uint32_t holdFrames = 0;     // curr==true のときの連続押下フレーム数（1..）, curr==false のときは 0
    uint32_t lastHoldOnRelease = 0; // 直近のリリース時に押されていたフレーム数（release イベント時に更新）
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
    bool leftButton = false;
    bool rightButton = false;
    bool middleButton = false;
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
    /// エミッターはAABB型か球型か
    // true = AABB　false = 球
    bool emitterShape = true;
    /// 全パーティクルがtarget方向に向かうかエミッターとtargetの垂直方向に向かうか
    // trueなら垂直方向、falseならtarget方向
    bool targetDirection = true;
    /// エミッター内部でも発生するか外殻上でのみ発生するか
    // trueなら内部でも発生、falseなら外殻のみ
    bool spawnInsideEmitter = true;
    // ビルボードか否か
    bool toCamera = false;
};

enum class LineType
{
    Line,
    BezierCurve,
    SplineCurve,
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

