#include "Object3d.hlsli"


// TransformationMatrix: レジスタ b1 に設定
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b1);

// グリッド定数 (ルートシグネチャのレジスタ2番に対応)
ConstantBuffer<GridConstants> gGridConstants : register(b2);


VertexOutput main(uint vertexID : SV_VertexID)
{
    
    VertexOutput output;

    // フルスクリーンクアッドの頂点を生成
    // グリッドは通常、XY平面に描画されるため、Zは0または固定値
    float2 uv = float2((vertexID << 1) & 2, (vertexID & 2));
    float2 screenPos = uv * 2.0f - 1.0f;
    output.SV_Position = float4(screenPos.x, -screenPos.y, 0.0f, 1.0f); // Yを反転してDirectXのクリップ空間に合わせる

    // カメラのワールド位置からレイを生成
    float4 worldPos = mul(output.SV_Position, inverse(gTransformationMatrix.World));
    output.WorldPos = worldPos.xyz / worldPos.w;

    return output;
}