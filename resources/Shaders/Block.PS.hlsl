
#include "Block.hlsli"

// サンプラーのレジスタ番号を s0 に設定
SamplerState gSampler : register(s0);
// テクスチャのレジスタ番号を t0 に設定
Texture2D<float32_t4> gTexture0 : register(t0); // ベーステクスチャ
Texture2D<float32_t4> gTexture1 : register(t1); // 破壊テクスチャ
// Material: レジスタ b0 に設定
ConstantBuffer<Material> gMaterial0 : register(b0);
// DirectionalLight: レジスタ b2 に設定
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);
// Camera: レジスタ b3 に設定
ConstantBuffer<Camera> gCamera : register(b3);

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial0.uvTransform);
    float2 uv = transformedUV.xy / transformedUV.w;
    float4 transformedUV2 = mul(float32_t4(input.texcoord2, 0.0f, 1.0f), gMaterial0.uvTransform);
    float2 uv2 = transformedUV2.xy / transformedUV2.w;

    float4 baseColor = gTexture0.Sample(gSampler, uv);
    float4 destroyColor = gTexture1.Sample(gSampler, uv2);

    // 破壊テクスチャのアルファでブレンド
    float overlayAlpha = destroyColor.a;
    float3 combinedColor = lerp(baseColor.rgb, destroyColor.rgb, overlayAlpha);

    float3 diffuse = combinedColor * gMaterial0.color.rgb;

    if (gDirectionalLight.mode == 2) // ハーフランバート
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        diffuse *= gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
    }
    else if (gDirectionalLight.mode == 1) // ランバート
    {
        float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        diffuse *= gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
    }
    // else → ライティングなし（diffuse はそのまま）

    float3 specular = { 0.0f, 0.0f, 0.0f };
    if (gDirectionalLight.phong)
    {
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        float3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
        float RdotE = dot(reflectLight, toEye);
        float specularPow = pow(saturate(RdotE), gMaterial0.shininess);

        specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
    }

    PixelShaderOutput output;
    output.color.rgb = diffuse + specular;
    output.color.a = gMaterial0.color.a * baseColor.a;

    return output;
}