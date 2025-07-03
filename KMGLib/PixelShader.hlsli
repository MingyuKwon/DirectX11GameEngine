#include "ShaderShared.hlsl"

float4 PS(PS_INPUT input) : SV_Target
{    
    float4 baseColor = txDiffuse.Sample(samLinear, input.Tex);
    float4 normalSample = txNormal.Sample(samLinear, input.Tex);
    
    float3 normalTS = normalize(normalSample.rgb * 2.0f - 1.0f);
    
    float3 finalColor = float3(0.5, 0.5, 0.5);

    float3 lightDir;
    if (type == 0) // Directional
    {
        lightDir = -normalize(direction);
    }
    else
    {
        lightDir = normalize(position - input.Pos.xyz);
    }
    
    finalColor += color * intensity * saturate(dot(normalTS, lightDir));

    //return float4(baseColor.rgb * finalColor, baseColor.a);

    
    float3x3 TBN = float3x3(
       normalize(input.Tangent.xyz),
       normalize(input.Binormal.xyz),
       normalize(input.Normal.xyz)
    );
    
    float3 T = normalize(input.Tangent);
    float3 N = normalize(input.Normal);
    float3 B = normalize(cross(N, T)); // handedness °öÇØµµ µÊ

    return float4(0.5 * (T + 1), 1); 
    return float4(0.5 * (B + 1), 1); 
    return float4(0.5 * (N + 1), 1); 

        
    float3 normalWS = normalize(mul(TBN, normalTS));

    float NdotL = saturate(dot(normalWS, lightDir));
   
    finalColor += color * intensity * NdotL;
    
    return float4(baseColor.rgb * finalColor, baseColor.a);
}