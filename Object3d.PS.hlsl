#include "object3d.hlsli"

SamplerState gSampler : register(s0); // サンプラーのレジスタ番号を s0 に設定
Texture2D<float32_t4> gTexture0 : register(t0); // テクスチャのレジスタ番号を t0 に設定
ConstantBuffer<Material> gMaterial0 : register(b0); // マテリアルのレジスタ番号を b0 に設定
ConstantBuffer<DirectionalLigft> gDirectionalLight : register(b1); 


PixelShaderOutput main(VertexShaderOutput input)
{
    // テクスチャカラーをサンプリング
    float32_t4 textureColor = gTexture0.Sample(gSampler, input.texcoord);
    // 出力カラーを計算
    PixelShaderOutput output;
    // Lightingの計算
    if (gMaterial0.enableLighting != 0)
    {
        float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        output.color = gMaterial0.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
    }
    else
    {
        output.color = gMaterial0.color * textureColor;
    }
    
    return output;
}