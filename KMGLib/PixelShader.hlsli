#include "ShaderShared.hlsl"

float4 PS(PS_INPUT input) : SV_Target
{
    float4 texColor = txDiffuse.Sample(samLinear, input.Tex);
    return texColor * input.Color; // ÅØ½ºÃ³ * ¹öÅØ½º ÄÃ·¯ (°ö¼À È¥ÇÕ)
}