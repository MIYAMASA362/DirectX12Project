cbuffer ObjectBuffer : register(b0)
{
    float4x4 world;
}

cbuffer CameraBuffer : register(b1)
{
    float4x4 view;
    float4x4 projection;
}

struct VSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float4 color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    matrix wvp;
    wvp = mul(world, view);
    wvp = mul(wvp, projection);
    
    PSInput result;
    result.position = mul(input.position, wvp);
    result.normal = input.normal;
    result.color = input.color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
