// Object3d.hlsli


struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};
struct VertexShaderOutput
{
    float32_t4 position : SV_Position;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};
struct VertexOutput
{
    float32_t4 SV_Position : SV_POSITION;
    float32_t3 WorldPos : WORLD_POS;
};
struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};
struct TransformationMatrix
{
    float32_t4x4 WVP;
    float32_t4x4 World;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

struct GridConstants
{
    float32_t4 gridColor; // グリッドのメインカラー (RGBA)
    float32_t4 subGridColor; // サブグリッドのカラー (RGBA)
    float32_t gridSize; // メイングリッドのサイズ (例: 1.0fで1m間隔)
    float32_t subGridSize; // サブグリッドのサイズ (例: 0.1fで10cm間隔)
    float32_t lineThickness; // メイングリッドの線の太さの乗数
    float32_t subLineThickness; // サブグリッドの線の太さの乗数
    float32_t minDistance; // グリッドの描画開始距離
    float32_t maxDistance; // グリッドの描画終了距離
    float32_t4 padding; // 16バイトアラインメントのためのパディング
};