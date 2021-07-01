#include "Transform.hlsl"

struct VSOut
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

VSOut main(float4 pos : Position, float3 normal : Normal )
{
    VSOut vso;
    vso.pos = pos;
    vso.normal = mul(normal, (float3x3) model);
    return vso;
}
