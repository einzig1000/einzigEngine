#include "Block.hlsli"

// サンプラーのレジスタ番号を s0 に設定
SamplerState gSampler : register(s0);
// テクスチャのレジスタ番号を t0 に設定
Texture2D<float32_t4> gTexture0 : register(t0); // ベーステクスチャ
//Texture2D<float32_t4> gTexture1 : register(t1); // 破壊テクスチャ
Texture2DArray<float32_t4> gBreakTexture : register(t1);
// Color: レジスタ t4 に設定
StructuredBuffer<float32_t4> gColor0 : register(t3);
// インスタンスごとの破壊テクスチャレイヤー
StructuredBuffer<uint> gBreakLayer : register(t4);

PixelShaderOutput main(VertexShaderOutput input)
{
    // UV同次座標
 //   PixelShaderOutput output;
 //   float32_t4 transformedUV = float32_t4(input.texcoord, 0.0f, 1.0f);
	//float32_t4 transformedUV2 = float32_t4(input.texcoord2, 0.0f, 1.0f);
 //   float32_t4 textureColor = gTexture0.Sample(gSampler, transformedUV.xy);
	//float32_t4 textureColor2 = gTexture1.Sample(gSampler, transformedUV2.xy);

 //   float useTex2 = step(1e-5, textureColor2.a); // 0 or 1
 //   float4 blended = lerp(textureColor, textureColor2, useTex2);
 //   output.color = gColor0[input.instancedID] * blended;

 //   if (output.color.a <= 1e-5) discard;
 //   return output;

    PixelShaderOutput output;
    float4 baseColor = gTexture0.Sample(gSampler, input.texcoord);
    uint layer = gBreakLayer[input.instancedID];
    float4 breakColor = gBreakTexture.Sample(gSampler, float3(input.texcoord2, (float)layer));

    float useBreak = step(1e-5, breakColor.a);
    float4 blended = lerp(baseColor, breakColor, useBreak);
    output.color = gColor0[input.instancedID] * blended;

    if (output.color.a <= 1e-5) discard;
    return output;
}