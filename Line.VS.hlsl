// 共通構造体の定義をインクルード
#include "object3d.hlsli"

//--------------------------------------------------------------------------------------
// 定数バッファ
//--------------------------------------------------------------------------------------
// TransformationMatrix: レジスタ b0
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

// Material: レジスタ b1 (ピクセルシェーダーへの入力色として利用)
ConstantBuffer<Material> gMaterial : register(b1);

// DirectionalLight: レジスタ b2 (線描画では通常使わないが、ルートシグネチャに合わせるため定義)
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

//--------------------------------------------------------------------------------------
// 頂点シェーダー (VS) エントリーポイント
//--------------------------------------------------------------------------------------
VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    // WVP行列による座標変換
    output.position = mul(input.position, gTransformationMatrix.WVP);
    
    // ピクセルシェーダーに渡す色として、マテリアルの色を設定
    // VertexShaderOutput に色メンバーがないため、Texcoord または Normal を流用することも考えられますが、
    // 汎用性を考慮し、ここでは VertexShaderOutput を拡張して色情報を渡すことを推奨します。
    // もし VertexShaderOutput を変更できない場合、ピクセルシェーダーで直接 gMaterial.color を参照することになります。
    // ここでは、VertexShaderOutput に color メンバーを追加することを前提とします。
    // (追記: 以前の定義では VertexShaderOutput に color はありませんでした。
    //       そのため、Line.PS.hlsl で gMaterial.color を直接参照する形にします。
    //       以下にその対応を追加します。)

    // 線描画では通常、テクスチャ座標と法線は不要ですが、
    // VertexShaderOutput構造体の定義に合わせてダミー値を設定します。
    // 不要な計算はコンパイラの最適化で取り除かれます。
    output.texcoord = float32_t2(0.0f, 0.0f);
    output.normal = float32_t3(0.0f, 0.0f, 1.0f); // ダミーの法線 (正規化済み)

    return output;
}