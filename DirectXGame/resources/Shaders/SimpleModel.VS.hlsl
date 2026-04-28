// Object3d.VS.hlsl
struct VSInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

cbuffer ViewProjectionMatrix : register(b0)
{
    float4x4 vp;
}

cbuffer WorldMatrix : register(b1)
{
    float4x4 world;
}

VSOutput main(VSInput input)
{
    VSOutput output;
    output.position = mul(input.position, world);
    output.position = mul(output.position, vp);
    output.texCoord = input.texCoord;
    output.normal = input.normal;
    return output;
}
