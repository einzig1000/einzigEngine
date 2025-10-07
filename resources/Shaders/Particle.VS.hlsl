// Object3d.VS.hlsl

#include "Particle.hlsli"

// TransformationMatrix: レジスタ b1 に設定
StructuredBuffer <TransformationMatrix> gTransformationMatrix : register(b1);

VertexShaderOutput main(VertexShaderInput input, uint32_t instancedID : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix[instancedID].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformationMatrix[instancedID].World));
    return output;
}