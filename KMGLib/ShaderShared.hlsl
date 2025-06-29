
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer cbChangeOnActor : register(b0)
{
    matrix World;
};

cbuffer cbChangeOnPlayer : register(b1)
{
    matrix View;
};

cbuffer cbChangeOnResize : register(b2)
{
    matrix Projection;
};



struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Normal : NORMAL;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Normal : NORMAL;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD0;
};