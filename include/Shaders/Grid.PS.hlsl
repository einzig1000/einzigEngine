#include "Object3d.hlsli"

// グリッド定数 (ルートシグネチャのレジスタ2番に対応)
ConstantBuffer<GridConstants> gGridConstants : register(b2);

float4 main(VertexOutput input) : SV_TARGET
{
    float3 worldPos = input.WorldPos;

    // メイングリッド
    float2 mainGrid = 1.0f - abs(frac(worldPos.xz / gGridConstants.gridSize) * 2.0f - 1.0f);
    float mainLine = saturate(min(mainGrid.x, mainGrid.y) * gGridConstants.lineThickness);

    // サブグリッド
    float2 subGrid = 1.0f - abs(frac(worldPos.xz / gGridConstants.subGridSize) * 2.0f - 1.0f);
    float subLine = saturate(min(subGrid.x, subGrid.y) * gGridConstants.subLineThickness);

    // 距離によるフェード（cameraPosが必要なら別途追加）
    float fade = 1.0f; // 仮にフェードなしとする

    // 色とアルファの合成
    float3 color = gGridConstants.gridColor.rgb;
    float alpha = (mainLine * gGridConstants.gridColor.a + subLine * gGridConstants.subGridColor.a) * fade;

    //color = float3(1.0f, 1.0f, 1.0f);
    //alpha = 1.0f;
    return float4(color, alpha);
}
