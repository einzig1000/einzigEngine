// Line.VS.hlsl

#include "Object3d.hlsli"

//--------------------------------------------------------------------------------------
// 定数バッファ
//--------------------------------------------------------------------------------------
// TransformationMatrix: レジスタ b1
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b1);

// Material: レジスタ b1 (ピクセルシェーダーへの入力色として利用)
ConstantBuffer<Material> gMaterial : register(b0);

// DirectionalLight: レジスタ b2 (線描画では通常使わないが、ルートシグネチャに合わせるため定義)
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

//--------------------------------------------------------------------------------------
// 頂点シェーダー (VS) エントリーポイント
//--------------------------------------------------------------------------------------
VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output{};
    
    // WVP行列による座標変換
    output.position = mul(input.position, gTransformationMatrix.WVP);
    
    // 線にUV座標と法線は必要ないけど、VertexShaderOutput構造体に合わせて適当な値をうなら
    output.texcoord = float2(0.0f, 0.0f);
    output.normal = float3(0.0f, 0.0f, 1.0f); 

    return output;
}
