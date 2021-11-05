#include "../Common/Transform.hlsl"

cbuffer LightViewProjection
{
    row_major matrix lightView;
    row_major matrix lightProj;
}; 


cbuffer SpotLightViewProjection
{
    row_major matrix spotLightView;
    row_major matrix spotLightProj;
}; 


struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 lightViewPos : Position2;
    float4 spotLightViewPos : Position3;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord)
{
    VSOut vso;
    vso.viewPos = pos;
    vso.viewNormal = mul(n, (float3x3) model);
    vso.tan = mul(tan, (float3x3) modelView);
    vso.bitan = mul(bitan, (float3x3) modelView);    
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.lightViewPos = mul(float4(pos, 1.0f), model);
    vso.lightViewPos = mul(vso.lightViewPos, lightView);
    vso.lightViewPos = mul(vso.lightViewPos, lightProj);
    vso.spotLightViewPos = mul(float4(pos, 1.0f), model);
    vso.spotLightViewPos = mul(vso.spotLightViewPos, spotLightView);
    vso.spotLightViewPos = mul(vso.spotLightViewPos, spotLightProj);
    vso.tc = tc;
    return vso;
}