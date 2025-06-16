// 共通構造体の定義をインクルード
#include "object3d.hlsli"

//--------------------------------------------------------------------------------------
// 定数バッファ
//--------------------------------------------------------------------------------------
// Material: レジスタ b1 (ここから直接色を取得)
ConstantBuffer<Material> gMaterial : register(b1);

// DirectionalLight: レジスタ b2 (線描画では通常使わないが、ルートシグネチャに合わせるため定義)
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