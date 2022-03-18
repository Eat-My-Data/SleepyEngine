#include "../Common/ShaderOps.hlsli"
#include "../Common/LightVectorData.hlsli"
#include "../Common/PointLight.hlsli"
#include "../Common/SpotLight.hlsli"
#include "../Common/PShadow.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 materialColor;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float4 spos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specular;

    // shadow map test
    const float shadowLevel = Shadow(spos);
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse
    diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular
    specular = Speculate(
        diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal,
        lv.vToL, viewFragPos, att, specularGloss
    );
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
    // ========================================================= DIRECTIONAL LIGHT ==================================================
    // shadow map test
    const float shadowLevel_directional = 1; //Shadow(spos);
    const LightVectorData lv_directional = CalculateLightVectorData(spotLightPos, viewFragPos);
	// attenuation
    const float att_directional = 0.4f; //Attenuate(attConst, attLin, attQuad, lv_directional.distToL);
	// diffuse light
    float3 diffuse_directional = Diffuse(diffuseColor, diffuseIntensity, att_directional, lv_directional.dirToL, viewNormal);
    // specular reflected
    float3 specularReflected_directional = Speculate(
        diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal,
        lv_directional.vToL, viewFragPos, att_directional, specularGloss
    );
    // scale by shadow level
    diffuse_directional *= shadowLevel_directional;
    specularReflected_directional *= shadowLevel_spot;
    // ========================================================= DIRECTIONAL LIGHT ==================================================
    // final color
    return float4(saturate((diffuse + diffuse_spot + diffuse_directional + ambient) * materialColor + specular + specularReflected_spot + specularReflected_directional), 1.0f);
}