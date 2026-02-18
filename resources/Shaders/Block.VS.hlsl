// Block.VS.hlsl

#include "Block.hlsli"

// 各インスタンスのワールド座標
StructuredBuffer<WorldMatrix> gWorldMatrix : register(t0);
// インスタンスごとの破壊アトラスのタイル番号
StructuredBuffer<uint> gBreakTile : register(t1);
// インスタンスごとのベースアトラスのタイル番号
StructuredBuffer<uint> gBaseTile : register(t2);
// 共通のビュー射影行列
ConstantBuffer<ViewProjectionMatrix> gViewProjection : register(b0);
// アトラス情報
ConstantBuffer<AtlasInfo> gBreakAtlasInfo : register(b1);
ConstantBuffer<AtlasInfo> gBaseAtlasInfo : register(b2);

// AtlasInfoから欲しいタイルのUVを計算する
static float2 ComputeAtlasUV(float2 uvInInner01, uint tileIndex, AtlasInfo info)
{
    // ターゲットタイルの左上座標を取得
	uint tileX = 0; // info.atlasCols=1固定なのでXは常に0
	uint tileY = tileIndex; // タイル番号=行番号


    //// uvPacked.x = faceId + u(0..1)
    //float faceF = floor(uvPacked.x);
    //uint faceId = (uint)faceF;
    //float u = uvPacked.x - faceF;
    //
    //faceId = min(faceId, info.facesPerBlock - 1);
    //float2 uv = saturate(float2(u, uvPacked.y));
    //
    //// ブロック行の左上px（行の高さ=faceStrideY）
    //uint2 blockOriginPx = uint2(0, blockIndex * info.faceStrideY);
    //
    //// 面の左上px（面は横に並ぶ）
    //uint2 faceOriginPx = blockOriginPx + uint2(faceId * info.faceStrideX, 0);
    //
    //// 有効領域(16x16)の左上（pad内側）
    //uint2 innerOriginPx = faceOriginPx + uint2(info.padX, info.padY);
    //
    //float2 innerPx = float2(innerOriginPx) + uv * float2(info.innerSizeX, info.innerSizeY);
    //
    //// ピクセル中心
    //innerPx += 0.5f;
    //
    //return innerPx * info.invAtlasSize;
}

VertexShaderOutput main(VertexShaderInput input, uint32_t instancedID : SV_InstanceID)
{
    VertexShaderOutput output;

	// 座標変換
    float4 worldPos = mul(input.position, gWorldMatrix[instancedID].World);
    output.position = mul(worldPos, gViewProjection.ViewProjection);
    // 欲しいタイル番号取得
	uint baseTile = gBaseTile[instancedID];
    uint breakTile = gBreakTile[instancedID];
	// UV計算
    output.texcoord = ComputeAtlasUV(input.texcoord, baseTile, gBaseAtlasInfo);
    output.texcoord2 = ComputeAtlasUV(input.texcoord2, breakTile, gBreakAtlasInfo);
	// 法線変換
    output.normal = normalize(mul(input.normal, (float3x3)gWorldMatrix[instancedID].World));
	// インスタンスIDを出力
	output.instancedID = instancedID;

    return output;
}