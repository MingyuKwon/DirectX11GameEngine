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


cbuffer Light : register(b3)
{
    int type; // 0 = directional, 1 = point, 2 = spot
    float range;
    float intensity;
    float lightPad_1;
    
    float3 position;
    // 여기 자동으로 4패딩
    
    float3 direction;
    // 여기 자동으로 4패딩
    
    float4 color;
};

//cbuffer cbLights 
//{
//    Light lights[MAX_LIGHTS];
//    int lightCount;
//    float _pad0[3];
//}


struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Normal : NORMAL;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD0;
    float4 Tangent : TANGENT;
    float4 Binormal : BINORMAL;
};


struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Normal : NORMAL;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD0;
    float4 Tangent : TANGENT;
    float4 Binormal : BINORMAL;

};