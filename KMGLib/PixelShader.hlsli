#include "ShaderShared.hlsl"

float4 PS(PS_INPUT input) : SV_Target
{
    return input.Color; 
}