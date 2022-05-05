#include "../Common/Transform.hlsli"
#include "../Common/VShadow.hlsli"

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float2 tc : Texcoord;
    float4 shadowHomoPosPL : ShadowPosition;
    float4 shadowHomoPosSL : ShadowPosition2;
    float4 shadowHomoPosDL : ShadowPosition3;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tc : Texcoord)
{
    VSOut vso;
    vso.viewPos = pos; //(float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(n, (float3x3) model);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tc = tc;
    vso.shadowHomoPosPL = ToShadowHomoSpace(pos, model, shadowPosPL);
    vso.shadowHomoPosSL = ToShadowHomoSpace(pos, model, shadowPosSL);
    vso.shadowHomoPosDL = ToShadowHomoSpace(pos, model, shadowPosDL);
    return vso;
}