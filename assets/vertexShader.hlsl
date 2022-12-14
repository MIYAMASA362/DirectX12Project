cbuffer cbTansMatrix : register(b0)
{
    float4x4 local;
};

cbuffer cbSceneMatrix : register(b1)
{
    float4x4 world;
    float4x4 view;
    float4x4 wvp;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 Diffuse  : COLOR;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 Diffuse  : COLOR;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    float4 position = float4(input.Position, 1.0f);
    output.Position = mul(position, wvp);

    float4 normal = float4(input.Normal, 0.0f);
    output.Normal = mul(normal, local);

    output.TexCoord = input.TexCoord;

    output.Diffuse.rgb = 1.0;
    output.Diffuse.a = 1.0;

    return output;
}

