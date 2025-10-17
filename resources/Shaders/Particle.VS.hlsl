// Particle.VS.hlsl

#include "Particle.hlsli"

// TransformationMatrix: レジスタ t1 に設定
StructuredBuffer <ParticleInstance> gParticleInstance : register(t1);

VertexShaderOutput main(VertexShaderInput input, uint32_t instancedID : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gParticleInstance[instancedID].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, gParticleInstance[instancedID].World));
    return output;
}