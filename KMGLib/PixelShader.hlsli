#include "ShaderShared.hlsl"

float4 PS(PS_INPUT input) : SV_Target
{
    return float4(1, 0, 0, 1); // 강제로 빨간색 출력
}