#include "../Common/Transform.hlsl"

cbuffer LightViewProjection
{
    row_major matrix lightView;
    row_major matrix lightProj;
};  

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float4 lightViewPos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal)
{
    VSOut vso;
    vso.viewPos = pos;
    vso.viewNormal = mul(n, (float3x3) model);
    vso.lightViewPos = mul(float4(pos, 1.0f), model);
    vso.lightViewPos = mul(vso.lightViewPos, lightView);
    vso.lightViewPos = mul(vso.lightViewPos, lightProj);
    return vso;
}