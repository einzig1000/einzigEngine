// Object3d.hlsli


struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};
struct VertexShaderOutput
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};
struct VertexOutput
{
    float4 SV_Position : SV_POSITION;
    float3 WorldPos : WORLD_POS;
};
struct Material
{
    float4 color;
    int32_t enableLighting;
    float4x4 uvTransform;
};
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};
struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
};

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

struct GridConstants
{
    float4 gridColor;
    float4 subGridColor;
    float gridSize;
    float subGridSize;
    float lineThickness;
    float subLineThickness;
    float minDistance;
    float maxDistance;
};