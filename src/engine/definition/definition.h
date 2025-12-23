#pragma once
// 標準ライブラリ
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <numbers>

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
#define CHUNK_X 16
#define CHUNK_Y 16
#define CHUNK_Z 16
#define BLOCK_SIZE 1.0f
#define PLAYER_SPEED 0.1f

#define GRAVITY -0.006f



enum class BlockID
{
    Air,
    Stone,	// 石
    Glass,	// ガラス
    Dirt,	// 草なし土
    Lawn,	// 草付き土
    Wood,	// 木材
    Leaf,	// 葉っぱ



    MAX,
};
std::string EnumToString(BlockID id);

// ブロックごとの情報
struct Blockinfo
{
    BlockID type;
    int32_t durability;
};

// ゲームのフェーズ
enum class PHASE
{
    Phase_None,
    Phase_Test,
    Phase_Title,
    Phase_GameScene,
    Phase_StageSelect,
    Phase_GameClear,
};
std::string EnumToString(PHASE e);



#pragma region 演算

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

#pragma endregion


#pragma region 基盤構造体

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

struct Vector3int
{
    int x = 0;
    int y = 0;
    int z = 0;
    Vector3int operator+(const Vector3int& rhs) const
    {
        return Vector3int{ x + rhs.x, y + rhs.y, z + rhs.z };
    }
    Vector3int operator-(const Vector3int& rhs) const
    {
        return Vector3int{ x - rhs.x, y - rhs.y, z - rhs.z };
    }
    bool operator==(const Vector3int& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    bool operator!=(const Vector3int& rhs) const
    {
        return x != rhs.x || y != rhs.y || z != rhs.z;
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

struct Vector4int
{
    int x = 0, y = 0, z = 0, w = 0;
    Vector4int operator+(const Vector4int& rhs) const
    {
        return Vector4int{ x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
    }
    Vector4int operator-(const Vector4int& rhs) const
    {
        return Vector4int{ x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
    }
    bool operator==(const Vector4int& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
    bool operator!=(const Vector4int& rhs) const
    {
        return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
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
    // 任意軸回転行列
    static Matrix4x4 MakeRotateAxisMatrix(const Vector3& axis, float radian);
};

#pragma endregion


#pragma region 幾何構造体

// 球
struct Sphere
{
    Vector3 center;
    float radius = 1.0f;
};

// 楕円体
struct SphereXYZ
{
    Vector3 center;
    Vector3 radius = { 1.0f, 1.0f, 1.0f };
};

// 平面
struct Plane
{
    Vector3 normal; // 法線
    float distance = 0.0f;
};

// 三角形
struct Triangle
{
    Vector3 vertices[3];
};

// 軸平行境界ボックス(Axis Aligned Bounding Box)
struct AABB
{
    Vector3 min;
    Vector3 max;

    Vector3 center()const;
    // min,maxが入れ替わる可能性があれば毎フレーム飛び出したい
    void Fix();

    Vector3 GetCollisionDepth(const AABB& other)const;
};

// 線分
struct Line
{
    // 始点
    Vector3 origin;
    // 終点
    Vector3 end;
};

// 半直線
struct Ray
{
    // 始点
    Vector3 origin;
    // 終点ベクトル
    Vector3 diff;
};

// 直線
struct Segment
{
    // 始点ベクトル
    Vector3 origin;
    // 終点ベクトル
    Vector3 diff;
};

enum class PrimitiveType
{
    // 球
    Sphere,
    // 楕円体
    SphereXYZ,
    // 立方体
    AABB,
    // 四角形
    Plane,
    // 円
    Circle,
};
std::string EnumToString(PrimitiveType e);

#pragma endregion


#pragma region イージング構造体

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
std::string EnumToString(EaseType e);

struct EasingSetVector3
{
    Vector3 start;
    Vector3 end;
    bool easingFlag = 0;
    int flame = 0;
    int maxFrame = 0;
    EaseType easetype = EaseType::OUT_QUART;
};

struct EasingSetFloat
{
    float start;
    float end;
    bool easingFlag = 0;
    int flame = 0;
    int maxFrame = 0;
    EaseType easetype = EaseType::OUT_QUART;
};

#pragma endregion


#pragma region モデルデータ構造体

// 材質データ(今はテクスチャパスしかいれてない.質感とか追加するようになったら使うのかも)
struct MaterialData
{
    std::string textureFilePath;
};

// 頂点データ
struct VertexData
{
    Vector4 position;
    Vector2 texcoord;
    Vector3 normal;
};

// 頂点データと材質データ
struct ModelData
{
    std::vector<VertexData> vertices;
    MaterialData material;
};

// 変換情報
struct Transforms
{
    Vector3 scale = { 1,1,1 };
    Vector3 rotate = { 0,0,0 };
    Vector3 translate = { 0,0,0 };
};

struct VectorDynamics
{
    Vector3 value = { 1.0f,1.0f,1.0f };
    Vector3 velocity;
    Vector3 acceleration;
};

struct TransformationMatrix
{
    Matrix4x4 WVP;
    Matrix4x4 World;
};

// 3Dオブジェクトデータ
struct Object3D
{
    // モデルデータ（頂点データと材質データ）
    ModelData modelData;

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    UINT vertexBufferSize = 0;

    // AABB
    std::vector<AABB> aabb;

    // 識別ナンバー
    int32_t number = 0;

    // ファイルパス
    std::string filePath;
};

// テクスチャデータ
struct TextureData
{
    DirectX::TexMetadata metadata;
    DirectX::ScratchImage mipImage;
    int32_t number;
    std::string filePath;
    Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
};

#pragma endregion


#pragma region 入力構造体

struct KeyState
{
    bool curr = false;           // 今フレームの押下状態
    bool prev = false;           // 前フレームの押下状態
    uint32_t holdFrames = 0;     // 長押しフレーム数
    uint32_t lastHoldOnRelease = 0; // 直近のリリース時に押されていたフレーム数
};

struct mouseButtenState
{
    bool curr = false;           // 今フレームの押下状態
    bool prev = false;           // 前フレームの押下状態
    uint32_t holdFrames = 0;     // 長押しフレーム数
};

#pragma endregion


#pragma region 描画オプション構造体

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
std::string EnumToString(BlendMode e);

enum class LightMode
{
    None = 0,
    Lambert,
    HalfLambert,
};
std::string EnumToString(LightMode e);

struct DirectionalLight
{
    Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector3 direction = { 0.0f, -1.0f, 0.0f };
    float intensity = 1.0f;//輝度
    LightMode mode = LightMode::HalfLambert;
    bool phong = false;
};

struct DrawOptions
{
    // ワイヤーフレーム
    bool wireframe = false;
    // 固有ライトを使うか共有ライトを使うか
    bool useOwnLight = false;
    // ライト
    DirectionalLight dirLight;
    // ブレンドモード
    BlendMode blendMode = BlendMode::kBlendModeNormal;
};

struct Material
{
    Vector4 color;
    Matrix4x4 uvTransform;
    float shininess;
};

// スプライトのアンカー位置
enum class Anchor
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
std::string EnumToString(Anchor e);

#pragma endregion


#pragma region 衝突判定構造体

enum class CollisionResult
{
    非衝突,
    接触,
    衝突
};
std::string EnumToString(CollisionResult e);

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

enum class AABBFace
{
    NONE = -1,
    LEFT = 0,
    RIGHT = 1,
    BOTTOM = 2,
    TOP = 3,
    BACK = 4,
    FRONT = 5,
};
std::string EnumToString(AABBFace e);

struct CollisionPair
{
    // 軽い方
    int light = 0;
    // 重い方
    int heavy = 0;

    bool operator==(const CollisionPair& rhs) const
    {
        return light == rhs.light && heavy == rhs.heavy;
    }
    bool operator!=(const CollisionPair& rhs) const
    {
        return !(*this == rhs);
    }
};

struct CollisionAABBFace
{
    AABBFace light = AABBFace::NONE;
    AABBFace heavy = AABBFace::NONE;
};

struct CollisionInf
{
    CollisionPair IDpair;    // 衝突したオブジェクトの識別番号ペア
    CollisionPair AABBpair;  // 衝突したオブジェクトのAABBの番号ペア
    CollisionAABBFace face;  // 衝突したオブジェクトのAABBの衝突面ペア
    Vector3 depth;    // 浸入深度
};

#pragma endregion


#pragma region パーティクルデータ構造体

// パーティクル情報

struct ParticleResource
{
    uint32_t model = 0;
    uint32_t texture = 0;
};

struct ParticleEmitter
{
    AABB emitterAABB = { Vector3{ -1.0f, -1.0f, -1.0f }, Vector3{ 1.0f, 1.0f, 1.0f } };
    SphereXYZ emitterSphere;
    bool useSphereEmitter = false;  // 球体エミッターを使うかどうか
    bool emitFromInside = true;
};

struct ParticleTarget
{
    bool useTarget = false; // ターゲット方向に飛ばすかどうか
    bool spawnDependent = false; // 発生位置に依存した方向に飛ばすかどうか
    Vector3 target;
    float speed = 1.0f;      // 速度
    float spreadAngle = 0.0f; // 拡散角度
};

struct ParticleDensity
{
    int particlesPerEmission = 1;   // 1フレで生む数
    int emissionDelay = 10;         // 生成間隔フレーム
    int liveMax = 300;              // 寿命フレーム(マイナスの時は不老)
    uint32_t frame = 0;             // 経過フレーム
};

struct ParticleOption
{
    bool isBillboard = true;    // ビルボードかどうか
};

struct ParticleSRT
{
    bool isRandom_value = false; // trueならランダム生成
    Vector3 value = { 1.0f,1.0f,1.0f };
    AABB randomRange_value;
    bool isRandom_velocity = false; // trueならランダム生成
    Vector3 velocity;
    AABB randomRange_velocity;
    bool isRandom_acceleration = false; // trueならランダム生成
    AABB randomRange_acceleration;
    Vector3 acceleration;
};

struct ParticleMaterial
{
    uint32_t color = 0xFFFFFFFF;
    Matrix4x4 uvTransform;
};

struct ParticleInf
{
    ParticleResource resource;
    ParticleEmitter emitter;
    ParticleTarget target;
    ParticleDensity density;
    ParticleOption option;
    ParticleSRT scale;
    ParticleSRT rotate;
    ParticleSRT translate;
    ParticleMaterial material;
};

// パーティクルインスタンス情報
struct ParticleMonoInf
{
    ParticleSRT scale;
    ParticleSRT rotate;
    ParticleSRT translate;

    Matrix4x4 World;
    Matrix4x4 WVP;

    uint32_t liveTime = 0;
    Vector4 color;
    uint32_t isBillboard;
};

struct ParticleMonoInfGPU
{
    Matrix4x4 World;
    Matrix4x4 WVP;
    Vector4 color;
    uint32_t liveTime;
    uint32_t isBillboard;
    uint32_t padding[2];
};

#pragma endregion


#pragma region カメラ構造体

enum class CameraMode_FirstPerson_ThirdPerson
{
    // 一人称視点
    FirstPerson,
    // 三人称後方視点
    ThirdPerson_Back,
    // 三人称前方視点
    ThirdPerson_Front,
};


enum class CameraMode_ORBIT_FPS
{
    ORBIT,
    FPS
};
std::string EnumToString(CameraMode_ORBIT_FPS e);

#pragma endregion


#pragma region 方向


// 上下左右
enum class DirectionXY
{
    None = -1,
    Left = 0,
    Right = 1,
    Down = 2,
    Up = 3,
};
std::string EnumToString(DirectionXY e);

// 前後左右
enum class DirectionXZ
{
    None = -1,
    Left = 0,
    Right = 1,
    Back = 2,
    Front = 3,
};
std::string EnumToString(DirectionXZ e);

// 前後左右 + 斜め
enum class DirectionXZ8Way
{
    None = -1,
    Front = 0,
    FrontLeft = 1,
    Left = 2,
    BackLeft = 3,
    Back = 4,
    BackRight = 5,
    Right = 6,
    FrontRight = 7,
};
std::string EnumToString(DirectionXZ8Way e);

// 上下左右前後
enum class DirectionXYZ
{
    None = -1,
    Left = 0,
    Right = 1,
    Back = 2,
    Front = 3,
    Down = 4,
    Up = 5,
};
std::string EnumToString(DirectionXYZ e);

#pragma endregion

// GPU用カメラ構造体
struct CameraForGPU
{
    Vector3 worldPosition;
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

enum class LineType
{
    Line,
    BezierCurve,
    SplineCurve,
};
std::string EnumToString(LineType e);


struct SRVAllocation
{
    uint32_t index = UINT32_MAX;
    D3D12_CPU_DESCRIPTOR_HANDLE cpu{};
    D3D12_GPU_DESCRIPTOR_HANDLE gpu{};
};
