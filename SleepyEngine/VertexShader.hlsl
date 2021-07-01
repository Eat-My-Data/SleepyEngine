#include "Transform.hlsl"

struct VSOut
{
    float3 viewPos : SV_Position;
    float3 viewNormal : Normal;
};

VSOut main(float3 pos : Position, float3 n : Normal )
{
    VSOut vso;
    vso.viewPos = pos;
    vso.viewNormal = mul(n, (float3x3) model);
    return vso;
}
