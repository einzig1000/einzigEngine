// Object3d.PS.hlsl

#include "object3d.hlsli"

// サンプラーのレジスタ番号を s0 に設定
SamplerState gSampler : register(s0);
// テクスチャのレジスタ番号を t0 に設定
Texture2D<float32_t4> gTexture0 : register(t0);
// Material: レジスタ b0 に設定
ConstantBuffer<Material> gMaterial0 : register(b0);
// DirectionalLight: レジスタ b2 に設定
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

PixelShaderOutput main(VertexShaderOutput input)
{
    // UV同次座標
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial0.uvTransform);
    float2 uv = transformedUV.xy / transformedUV.w;

    PixelShaderOutput output;
    float32_t4 textureColor = gTexture0.Sample(gSampler, uv);


    if (gDirectionalLight.mode == 0)
    {
        // Lightingの計算
        if (gMaterial0.enableLighting != 0)
        {
            float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            output.color.rgb = gMaterial0.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
            output.color.a = gMaterial0.color.a * textureColor.a;
        }
        else
        {
            output.color = gMaterial0.color * textureColor;
        }
        return output;
    }
    else if (gDirectionalLight.mode == 1)
    {
        // Lightingの計算
        if (gMaterial0.enableLighting != 0)
        {
            float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
            output.color.rgb = gMaterial0.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
            output.color.a = gMaterial0.color.a * textureColor.a;
        }
        else
        {
            output.color = gMaterial0.color * textureColor;
        }
        return output;
    }
    else
    {
        output.color = gMaterial0.color * textureColor;
        return output;
    }
}