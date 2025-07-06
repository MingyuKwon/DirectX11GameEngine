#include "ShaderShared.hlsl"

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    float4 worldPos = mul(input.Pos, World);

    float3x3 worldRot = (float3x3) World;

    float3 worldNormal = normalize(mul(input.Normal.xyz, worldRot));
    float3 worldTangent = normalize(mul(input.Tangent.xyz, worldRot));
    float3 worldBinormal = normalize(mul(input.Binormal.xyz, worldRot));

    output.worldPos = worldPos;
    output.Pos = mul(worldPos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;

    output.Normal = float4(worldNormal, 0.0);
    output.Tangent = float4(worldTangent, 0.0);
    output.Binormal = float4(worldBinormal, 0.0);
    output.Color = input.Color;

    return output;
}