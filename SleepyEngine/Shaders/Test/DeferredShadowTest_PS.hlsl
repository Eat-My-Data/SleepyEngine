#include "../Common/ShaderOps.hlsli"
#include "../Common/LightVectorData.hlsli"
#include "../Common/PointLight.hlsli"
#include "../Common/PShadow.hlsli"
#include "../Deferred/DeferredPSOut.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex : register(t0);

SamplerState splr : register(s0);


DeferredPSOut main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord)
{
    DeferredPSOut psOut;
    psOut.color = tex.Sample(splr, tc);
    psOut.normal = float4(viewNormal, 1.0f);
    psOut.specular = float4(specularColor, 1.0f);
    return psOut;
}