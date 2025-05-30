#pragma once
// 標準ライブラリ
#include <vector>
#include <string>

// Windows/DirectX
#include <initguid.h>
#include <dxgidebug.h>
#include <d3d12.h>

// ライブラリリンク
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d12.lib")

// WRL
#include <wrl.h>

// 外部ライブラリ
#include "externals/DirectXTex/DirectXTex.h"

#define WIDTH 1280
#define HEIGHT 720

struct Vector2int
{
    int x = 0;
    int y = 0;
};

struct Vector2
{
    float x = 0;
    float y = 0;

    Vector2 operator-(const Vector2& rhs) const
    {
        return Vector2{ x - rhs.x, y - rhs.y };
    }
    Vector2 operator+(const Vector2& rhs) const
    {
        return Vector2{ x + rhs.x, y + rhs.y };
    }
    bool operator==(const Vector2& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
};

struct Vector3
{
    float x = 0;
    float y = 0;
    float z = 0;

    Vector3 operator-(const Vector3& rhs) const
    {
        return Vector3{ x - rhs.x, y - rhs.y, z - rhs.z };
    }
    Vector3 operator+(const Vector3& rhs) const
    {
        return Vector3{ x + rhs.x, y + rhs.y, z + rhs.z };
    }
    bool operator==(const Vector3& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
};

struct Vector4
{
    float x = 0, y = 0, z = 0, w = 0;

    Vector4 operator-(const Vector4& rhs) const
    {
        return Vector4{ x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
    }
    Vector4 operator+(const Vector4& rhs) const
    {
        return Vector4{ x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
    }
    bool operator==(const Vector4& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
};

struct Matrix3x3
{
    float m[3][3] = {
    { 0,0,0 },
    { 0,0,0 },
    { 0,0,0 } };
};

struct Matrix4x4
{
    float m[4][4] = {
    { 0,0,0,0 },
    { 0,0,0,0 },
    { 0,0,0,0 },
    { 0,0,0,0 } };
};

struct Sphere
{
    Vector3 center;
    float radius = 1;
};

struct Plane
{
    Vector3 normal; // 法線
    float distance;
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


struct Object3D
{
    // モデルデータ
    ModelData modelData;

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    UINT vertexBufferSize;

    // マテリアルデータ
    //Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
    //Material* materialData;

    // 変換行列
    Transforms transform;

    // ワールド・ビュー・プロジェクション行列
    //std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> transformationMatrixResource;
    //std::vector<TransformationMatrix*> transformationMatrixData;

    // テクスチャ
    //Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
    //D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

    // 識別ナンバー
    uint32_t number;
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

// ブロックのローカルAABB（中心(0,0,0)、サイズ1の立方体）
struct AABB
{
    Vector3 min;
    Vector3 max;
};