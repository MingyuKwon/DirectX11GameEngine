#include "ShaderShared.hlsl"

float4 PS(PS_INPUT input) : SV_Target
{
    float4 baseColor = txDiffuse.Sample(samLinear, input.Tex);

    float3 finalColor = float3(0.2, 0.2, 0.2);
    
    for (int i = 0; i < lightCount; i++)
    {
        float3 lightDir = lights[i].direction;
                        
        if (lights[i].type == 0)
        {
            lightDir = -normalize(lights[i].direction);
        }
        else
        {
            lightDir = normalize(lights[i].position - input.worldPos.xyz);
        }
        
        float NdotM = saturate(dot(input.Normal.xyz, lightDir));

        finalColor += lights[i].color * lights[i].intensity * NdotM;
    }

    return float4(baseColor.rgb * finalColor, baseColor.a);

}