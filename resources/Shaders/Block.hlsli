// Block.hlsli


struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t2 texcoord2 : TEXCOORD1;
    float32_t3 normal : NORMAL0;
};
struct VertexShaderOutput
{
    float32_t4 position : SV_Position;
    float32_t2 texcoord : TEXCOORD0;
    float32_t2 texcoord2 : TEXCOORD1;
    float32_t3 normal : NORMAL0;
    uint32_t instancedID : TEXCOORD2; // TEXCOORD2ではない
};


struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct Material
{
    float32_t4 color;
    float32_t4x4 uvTransform;
    float32_t shininess;
};
struct WorldMatrix
{
    float32_t4x4 World;
};

struct ViewProjectionMatrix
{
    float32_t4x4 ViewProjection;
};

struct DirectionalLight
{
    float32_t4 color; // 16 bytes
    float32_t3 direction; // 12 bytes
    float intensity; // 4 bytes → 合計32 bytes
    int mode; // 4 bytes
    bool phong; // 4 bytes（HLSLではboolもint扱い）
    float32_t2 padding; // 8 bytes → 合計16 bytes（アライメント調整）
};

struct Camera
{
    float32_t3 worldPosition;
};