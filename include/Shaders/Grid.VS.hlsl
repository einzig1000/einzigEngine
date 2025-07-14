
//#include "Object3d.hlsli"

// 定数バッファ (ルートシグネチャのレジスタ0番に対応)
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

// グリッド定数 (ルートシグネチャのレジスタ2番に対応)
ConstantBuffer<GridConstants> gGridConstants : register(b2);

struct VertexOutput
{
    float4 SV_Position : SV_POSITION;
    float3 WorldPos : WORLD_POS;
};

VertexOutput VSMain(uint vertexID : SV_VertexID)
{
    VertexOutput output;

    // フルスクリーンクアッドの頂点を生成
    // グリッドは通常、XY平面に描画されるため、Zは0または固定値
    float2 uv = float2((vertexID << 1) & 2, (vertexID & 2));
    float2 screenPos = uv * 2.0f - 1.0f;
    output.SV_Position = float4(screenPos.x, -screenPos.y, 0.0f, 1.0f); // Yを反転してDirectXのクリップ空間に合わせる

    // カメラのワールド位置からレイを生成
    float4 worldPos = mul(output.SV_Position, inverse(gTransformationMatrix.viewProjection));
    output.WorldPos = worldPos.xyz / worldPos.w;

    return output;
}