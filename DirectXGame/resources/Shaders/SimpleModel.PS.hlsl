// Object3d.PS.hlsl
struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

cbuffer ColorBuffer : register(b0)
{
    float4 color;
}

cbuffer TextureIndex : register(b1)
{
    int textureIndex;
};

Texture2D<float4> textures[] : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput output;
    float4 texColor = textures[textureIndex].Sample(gSampler, input.texcoord);
    output.color = color * texColor;
    return output;
}
