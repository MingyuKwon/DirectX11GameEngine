#include "ShaderShared.hlsl"

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    float4 worldPos = mul(input.Pos, World);
    float4 worldNormal = normalize(mul(input.Normal, World));
    float4 worldTangent = normalize(mul(input.Tangent, World));
    float4 worldBinormal = normalize(mul(input.Binormal, World));

    output.worldPos = worldPos;
    output.Pos = mul(worldPos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;

    output.Normal = worldNormal;
    output.Tangent = worldTangent;
    output.Binormal = worldBinormal;
    
    //output.Normal = input.Normal;
    //output.Tangent = input.Tangent;
    //output.Binormal = input.Binormal;

    
    output.Color = input.Color;

    return output;
}