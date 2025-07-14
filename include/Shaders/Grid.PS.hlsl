
#include "Object3d.hlsli"

// 定数バッファ (ルートシグネチャのレジスタ0番に対応)
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

// グリッド定数 (ルートシグネチャのレジスタ2番に対応)
ConstantBuffer<GridConstants> gGridConstants : register(b2);

struct VertexOutput
{
    float4 SV_Position : SV_POSITION;
    float3 WorldPos : WORLD_POS;
};

float4 PSMain(VertexOutput input) : SV_TARGET
{
    float3 worldPos = input.WorldPos;

    float3 color = gGridConstants.gridColor.rgb;

    // メイングリッド
    float2 mainGrid = 1.0f - abs(frac(worldPos.xz / gGridConstants.gridSize) * 2.0f - 1.0f);
    float mainGridLine = saturate(min(mainGrid.x, mainGrid.y) * gGridConstants.lineThickness);

    // サブグリッド
    float2 subGrid = 1.0f - abs(frac(worldPos.xz / gGridConstants.subGridSize) * 2.0f - 1.0f);
    float subGridLine = saturate(min(subGrid.x, subGrid.y) * gGridConstants.subLineThickness);

    // 距離によるフェード
    float dist = length(worldPos - gTransformationMatrix.cameraPos.xyz);
    float fade = saturate((gGridConstants.maxDistance - dist) / (gGridConstants.maxDistance - gGridConstants.minDistance));

    float finalAlpha = (mainGridLine * gGridConstants.gridColor.a + subGridLine * gGridConstants.subGridColor.a) * fade;

    return float4(color, finalAlpha);
}