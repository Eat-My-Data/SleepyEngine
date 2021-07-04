#include "Transform.hlsl"

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal)
{
    VSOut vso;
    vso.viewPos = pos;
    vso.viewNormal = mul(n, (float3x3) model);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    return vso;
}