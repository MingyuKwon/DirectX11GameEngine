#include "ShaderShared.hlsl"

float4 PS(PS_INPUT input) : SV_Target
{        
    float4 baseColor = txDiffuse.Sample(samLinear, input.Tex); 
    float4 normalSample = txNormal.Sample(samLinear, input.Tex);
    
    float3 normalTS = normalize(normalSample.rgb * 2.0f - 1.0f);

    float3 T = normalize(input.Tangent);
    float3 B = normalize(input.Binormal);
    float3 N = normalize(input.Normal);
    
    float3x3 TBN = float3x3(T, B, N);

    float3 normalWS = mul(normalTS, TBN);
    
    float3 finalColor = float3(0,0,0);
    
    for (int i = 0; i < lightCount; i++)
    {
        float3 lightDir = lights[i].direction;
        float attenuation = 1;
        
        if (lights[i].type == 0)
        {
            lightDir = -normalize(lights[i].direction);
        }
        else
        {
            float3 lightVec = lights[i].position - input.worldPos.xyz;
            float distance = length(lightVec);

            lightDir = normalize(lightVec);

            attenuation = 1.0 / (1 + 0.09 * distance + 0.032 * distance * distance);
        }
        
        float NdotM = saturate(dot(normalWS, lightDir));
        float NdotL = saturate(dot(N, lightDir));

        finalColor += lights[i].color * lights[i].intensity * attenuation * (NdotL + NdotM);
    }
    
    return float4(baseColor.rgb * finalColor, baseColor.a);

}