#include "ShaderShared.hlsl"

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = input.Pos;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;
    output.Tangent = input.Tangent;
    output.Binormal = input.Binormal;
    output.Normal = input.Normal;
    
    return output;
}