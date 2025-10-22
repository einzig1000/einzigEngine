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

struct TransformationMatrix
{
    float32_t4x4 WVP;
    float32_t4x4 World;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
    int mode;
};


//struct ParticleTransforms
//{
//    float32_t3 scale;
//    float32_t3 rotate;
//    float32_t3 translate;
//    float32_t3 velocity;
//    float32_t4x4 World;
//    float32_t4x4 WVP;
//    uint32_t liveTime;
//    uint32_t color;
//};

//struct ParticleInf
//{
//    ParticleTransforms transformationMatrix;
//    float32_t3 velocity;
//    int32_t liveTime;
//};

//struct ParticleInf
//{
//    float32_t3 scale;
//    float32_t3 rotate;
//    float32_t3 translate;
//    float32_t3 velocity;
//    float32_t4x4 World;
//    float32_t4x4 WVP;
//    uint32_t liveTime;
//    uint32_t color;
//};

struct ParticleInf
{
    float32_t3 scale;
    float32_t3 scaleVelocity;
    float32_t3 scaleAcceleration;
    
    float32_t3 rotate;
    float32_t3 rotateVelocity;
    float32_t3 rotateAcceleration;
    
    float32_t3 translate;
    float32_t3 translateVelocity;
    float32_t3 translateAcceleration;
    
    float32_t4x4 World;
    float32_t4x4 WVP;
    
    uint32_t liveTime;
    float32_t4 color;
    bool isBillboard;
};