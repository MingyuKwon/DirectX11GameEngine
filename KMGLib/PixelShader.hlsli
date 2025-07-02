#include "ShaderShared.hlsl"

// 시험용 광원

float4 PS(PS_INPUT input) : SV_Target
{
    float3 lightDirWS = float3(0.0f, 0.0f, -1.0f);

    float4 baseColor = txDiffuse.Sample(samLinear, input.Tex);
    float4 normalSample = txNormal.Sample(samLinear, input.Tex);
    
    // 노멀맵에서 가져온 RGB → [-1, 1] 범위 벡터로 변환
    float3 normalTS = normalize(normalSample.rgb * 2.0f - 1.0f);
    
    // 탱젠트 공간 → 월드 공간 변환
    float3x3 TBN = float3x3(
       normalize(input.Tangent),
       normalize(input.Binormal),
       normalize(input.Normal.xyz)
    );
        
    // 월드 노멀 결과
    float3 normalWS = normalize(mul(TBN, normalTS));
    
    // diffuse 결과
    float diffuse = saturate(dot(normalize(lightDirWS), normalWS));
    return float4(baseColor.rgb * diffuse, baseColor.a);
}