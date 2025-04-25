
struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
};struct VertexShaderOutput
{
    float32_t4 position : SV_Position;
    float32_t2 texcoord : TEXCOORD;
};

struct Material
{
    float32_t4 color;
};
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};
struct TransformationMatrix
{
    float32_t4x4 WVP;
};
