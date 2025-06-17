// Object3d.PS.hlsl

#include "object3d.hlsli"

//SamplerState gSampler : register(s0); // サンプラーのレジスタ番号を s0 に設定
//Texture2D<float32_t4> gTexture0 : register(t0); // テクスチャのレジスタ番号を t0 に設定
//ConstantBuffer<Material> gMaterial0 : register(b0); // マテリアルのレジスタ番号を b0 に設定
//ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1); 


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
    // テクスチャカラーをサンプリング
    float32_t4 textureColor = gTexture0.Sample(gSampler, transformedUV.xy);
    // 出力カラーを計算
    PixelShaderOutput output;
    // Lightingの計算
    if (gMaterial0.enableLighting != 0)
    {
        // NdotL : NとLのdot(内積)
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color = gMaterial0.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
    }
    else
    {
        output.color = gMaterial0.color * textureColor;
    }
    
    return output;
}