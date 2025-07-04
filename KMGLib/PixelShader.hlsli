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
        
        if (lights[i].type == 0)
        {
            lightDir = -normalize(lights[i].direction);
        }
        else
        {
            lightDir = normalize(lights[i].position - input.Pos.xyz);
        }
        
        float NdotM = saturate(dot(normalWS, lights[i].direction));
        float NdotL = saturate(dot(N, lights[i].direction));

        finalColor += lights[i].color * lights[i].intensity * (NdotL + NdotM);
    }

    

    return float4(baseColor.rgb * finalColor, baseColor.a);

}