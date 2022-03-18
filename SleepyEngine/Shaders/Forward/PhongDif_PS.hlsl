#include "../Common/ShaderOps.hlsli"
#include "../Common/LightVectorData.hlsli"
#include "../Common/PointLight.hlsli"
#include "../Common/SpotLight.hlsli"
#include "../Common/PShadow.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex : register(t0);

SamplerState splr : register(s0);


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 spos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specular;

    // shadow map test
    const float shadowLevel = 1; //Shadow(spos);
    // renormalize interpolated normal
    viewNormal = normalize(viewNormal);
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse
    diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
	// specular
    specular = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal, lv.vToL, viewFragPos, att, specularGloss);
        // scale by shadow level
    diffuse *= shadowLevel;
    specular *= shadowLevel;
    // ========================================================= SPOT LIGHT =========================================================
    // shadow map test
    const float shadowLevel_spot = 1; //Shadow(spos);
    const LightVectorData lv_spot = CalculateLightVectorData(spotLightPos, viewFragPos);
	// attenuation
    const float att_spot = Attenuate(attConst, attLin, attQuad, lv_spot.distToL);
	// diffuse light
    float3 diffuse_spot = Diffuse(diffuseColor, diffuseIntensity, att_spot, lv_spot.dirToL, viewNormal);
    // specular reflected
    float3 specularReflected_spot = Speculate(
        diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal,
        lv_spot.vToL, viewFragPos, att_spot, specularGloss
    );
    // scale by shadow level
    diffuse_spot *= shadowLevel_spot;
    specularReflected_spot *= shadowLevel_spot;
    // ========================================================= SPOT LIGHT =========================================================
	// final color
    return float4(saturate((diffuse + diffuse_spot + ambient) * tex.Sample(splr, tc).rgb + specular + specularReflected_spot), 1.0f);
}