// Particle.hlsli


struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};
struct VertexShaderOutput
{
    float32_t4 position : SV_Position;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};
struct ParticleInstance
{
    float32_t4x4 WVP; // 64 bytes
    float32_t4x4 World; // 64 bytes
    float32_t4 velocity; // 16 bytes (例: xyz=速度, w=未使用/寿命など)
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
    int mode;
};