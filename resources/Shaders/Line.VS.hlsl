// Line.VS.hlsl

#include "Line.hlsli"

//--------------------------------------------------------------------------------------
// 定数バッファ
//--------------------------------------------------------------------------------------
// TransformationMatrix: レジスタ b1
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b1);

// Material: レジスタ b1 (ピクセルシェーダーへの入力色として利用)
ConstantBuffer<Material> gMaterial : register(b0);

// DirectionalLight: レジスタ b2 (線描画では通常使わないが、ルートシグネチャに合わせるため定義)
//ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

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