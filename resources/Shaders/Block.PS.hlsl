#include "Block.hlsli"

// サンプラーのレジスタ番号を s0 に設定
SamplerState gSampler : register(s0);
// テクスチャのレジスタ番号を t0 に設定
Texture2D<float32_t4> gTexture0 : register(t0); // ベーステクスチャ
Texture2D<float32_t4> gTexture1 : register(t1); // 破壊テクスチャ
// Material: レジスタ b0 に設定
ConstantBuffer<Material> gMaterial0 : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    // UV同次座標
    PixelShaderOutput output;
    float32_t4 transformedUV = mul(gMaterial0.uvTransform, float32_t4(input.texcoord, 0.0f, 1.0f));
	float32_t4 transformedUV2 = mul(gMaterial0.uvTransform, float32_t4(input.texcoord2, 0.0f, 1.0f));
    float32_t4 textureColor = gTexture0.Sample(gSampler, transformedUV.xy);
	float32_t4 textureColor2 = gTexture1.Sample(gSampler, transformedUV2.xy);
	textureColor += textureColor2;
    output.color = gMaterial0.color * textureColor;
    if (output.color.a <= 1e-5) discard;
    return output;
}