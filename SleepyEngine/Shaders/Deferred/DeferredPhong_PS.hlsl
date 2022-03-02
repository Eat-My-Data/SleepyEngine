#include "../Common/ShaderOps.hlsli"
#include "../Common/LightVectorData.hlsli"
#include "../Common/PointLight.hlsli"
#include "../Common/PShadow.hlsli"
#include "DeferredPSOut.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 materialColor;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};


DeferredPSOut main(float3 viewFragPos : Position, float3 viewNormal : Normal, float4 spos : ShadowPosition)
{
    DeferredPSOut psOut;
    psOut.color = float4(materialColor, 1.0f);
    psOut.normal = float4(viewNormal, 1.0f);
    psOut.specular = float4(specularColor, 1.0f);
    return psOut;
}