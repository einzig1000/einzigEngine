// Particle.VS.hlsl

#include "Particle.hlsli"

// TransformationMatrix: レジスタ t1 に設定
StructuredBuffer <ParticleMonoInf> gParticleMonoInf : register(t1);

VertexShaderOutput main(VertexShaderInput input, uint32_t instancedID : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gParticleMonoInf[instancedID].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3)gParticleMonoInf[instancedID].World));
    return output;
}