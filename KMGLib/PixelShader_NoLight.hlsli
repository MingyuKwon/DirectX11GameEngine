#include "ShaderShared.hlsl"

float4 PS(PS_INPUT input) : SV_Target
{
    float4 baseColor = txDiffuse.Sample(samLinear, input.Tex);
   
    return input.Color;

}