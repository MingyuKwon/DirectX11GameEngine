#include "ShaderShared.hlsl"

float4 PS(PS_INPUT input) : SV_Target
{    
    float4 baseColor = txDiffuse.Sample(samLinear, input.Tex);
    float4 normalSample = txNormal.Sample(samLinear, input.Tex);
    
    float3 normalTS = normalize(normalSample.rgb * 2.0f - 1.0f);
    
    float3x3 TBN = float3x3(
       normalize(input.Tangent),
       normalize(input.Binormal),
       normalize(input.Normal.xyz)
    );
        
    
    float3 normalWS = normalize(mul(TBN, normalTS));
    
    float3 finalColor = float3(0, 0, 0);
    
    float3 lightDir;
    if (type == 0) // Directional
        lightDir = -normalize(direction);
    else
        lightDir = normalize(position - input.Pos.xyz);

    float NdotL = saturate(dot(normalWS, lightDir));
    finalColor += color * intensity * NdotL;
    
    return float4(baseColor.rgb * finalColor, baseColor.a);
}