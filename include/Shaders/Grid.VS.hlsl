#include "Object3d.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b1);

//VertexOutput main(VertexShaderInput input)
//{
//    VertexOutput output;
//    output.SV_Position = mul(input.position, gTransformationMatrix.WVP);
//    output.WorldPos = mul(input.position, gTransformationMatrix.World).xyz;
//    return output;
//}

VertexOutput main(uint vertexId : SV_VertexID)
{
    float2 pos[3] = { float2(-1,-1), float2(-1,1), float2(1,-1) };
    VertexOutput output;
    output.SV_Position = float4(pos[vertexId], 0, 1);
    output.WorldPos = float3(pos[vertexId], 0);
    return output;
}