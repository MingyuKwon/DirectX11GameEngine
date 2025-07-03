#define MAX_LIGHTS 8

Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
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


struct Light
{
    int type; // 0 = directional, 1 = point, 2 = spot
    float3 _pad0;

    float3 position;
    float _pad1;

    float3 direction;
    float range;

    float intensity;
    float3 color;
};

cbuffer cbLights : register(b3)
{
    Light lights[MAX_LIGHTS];
    int lightCount;
    float _pad0[3];
}


struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Normal : NORMAL;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD0;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
};


struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Normal : NORMAL;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD0;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;

};