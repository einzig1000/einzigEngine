// Line.VS.hlsl

#include "Line.hlsli"

//--------------------------------------------------------------------------------------
// 定数バッファ
//--------------------------------------------------------------------------------------
// TransformationMatrix: レジスタ b1
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b1);

// Material: レジスタ b1 (ピクセルシェーダーへの入力色として利用)
ConstantBuffer<Material> gMaterial : register(b0);

//--------------------------------------------------------------------------------------
// 頂点シェーダー (VS) エントリーポイント
//--------------------------------------------------------------------------------------
LineShaderOutput main(LineShaderInput input)
{
    
    LineShaderOutput output;
    
    // WVP行列による座標変換
    output.position = mul(input.position, gTransformationMatrix.WVP);

    return output;
}