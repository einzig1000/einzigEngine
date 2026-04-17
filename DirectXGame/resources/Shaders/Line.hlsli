// Line.hlsli



struct LineShaderInput
{
    float32_t4 position : POSITION;
};

struct LineShaderOutput
{
    float32_t4 position : SV_POSITION;
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