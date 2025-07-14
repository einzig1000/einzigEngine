// Line.PS.hlsl

//#include "Object3d.hlsli"

//--------------------------------------------------------------------------------------
// 定数バッファ
//--------------------------------------------------------------------------------------
// Material: レジスタ b0。これはルートパラメータ0に対応します。
ConstantBuffer<Material> gMaterial : register(b0);

// DirectionalLight: レジスタ b2
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);


//--------------------------------------------------------------------------------------
// ピクセルシェーダー (PS) エントリーポイント
//--------------------------------------------------------------------------------------
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // マテリアルカラーをそのまま出力
    output.color = gMaterial.color;
    
    return output;
}