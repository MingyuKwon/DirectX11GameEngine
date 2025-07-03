#include "ShaderShared.hlsl"

// ½ÃÇè¿ë ±¤¿ø

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
    
    float3 finalColor = float3(lightCount * 0.5, lightCount * 0.5, lightCount * 0.5);

    for (int i = 0; i < lightCount; ++i)
    {
        Light l = lights[i];

        float3 lightDir;
        if (l.type == 0) // Directional
            lightDir = -normalize(l.direction);
        else
            lightDir = normalize(l.position - input.Pos.xyz);

        float NdotL = saturate(dot(normalWS, lightDir));
        finalColor += l.color * l.intensity * NdotL;
    }
    
    return float4(baseColor.rgb * finalColor, baseColor.a);
}