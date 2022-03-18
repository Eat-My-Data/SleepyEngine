#include "../Common/ShaderOps.hlsli"
#include "../Common/LightVectorData.hlsli"
#include "../Common/PointLight.hlsli"
#include "../Common/SpotLight.hlsli"
#include "../Common/DirectionalLight.hlsli"
#include "../Common/PShadow.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex : register(t0);
Texture2D spec : register(t1);

SamplerState splr : register(s0);


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 spos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specularReflected;
    // ========================================================= POINT LIGHT ========================================================
    // shadow map test
    const float shadowLevel = 1; //Shadow(spos);
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
    // specular parameters
    float specularPowerLoaded = specularGloss;
    const float4 specularSample = spec.Sample(splr, tc);
    float3 specularReflectionColor;
    if (useSpecularMap)
    {
        specularReflectionColor = specularSample.rgb;
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    if (useGlossAlpha)
    {
        specularPowerLoaded = pow(2.0f, specularSample.a * 13.0f);
    }
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse light
    diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular reflected
    specularReflected = Speculate(
        diffuseColor * specularReflectionColor, specularWeight, viewNormal,
        lv.vToL, viewFragPos, att, specularPowerLoaded
    );
    // scale by shadow level
    diffuse *= shadowLevel;
    specularReflected *= shadowLevel;
    // ========================================================= POINT LIGHT ========================================================
    // ========================================================= SPOT LIGHT =========================================================
    float3 spotToFrag = spotLightPos - viewFragPos.xyz;
    // shadow map test
    const float shadowLevel_spot = 1; //Shadow(spos);
    const LightVectorData lv_spot = CalculateLightVectorData(spotLightPos, viewFragPos);
	// attenuation
    const float att_spot = AttenuateSpot(spotToFrag, lv_spot.distToL);
	// diffuse light
    float3 diffuse_spot = Diffuse(spotLightColor, spotLightIntensity, att_spot, lv_spot.dirToL, viewNormal);
    // specular reflected
    float3 specularReflected_spot = Speculate(
        spotLightColor * spotLightIntensity * specularReflectionColor, specularWeight, viewNormal,
        lv_spot.vToL, viewFragPos, att_spot, specularPowerLoaded
    );
    // scale by shadow level
    diffuse_spot *= shadowLevel_spot;
    specularReflected_spot *= shadowLevel_spot;
    // ========================================================= SPOT LIGHT =========================================================
    // ========================================================= DIRECTIONAL LIGHT ==================================================
    // shadow map test
    const float shadowLevel_directional = 1; //Shadow(spos);
	// attenuation
    const float att_directional = 0.4f; //Attenuate(attConst, attLin, attQuad, lv_directional.distToL);
	// diffuse light
    float3 diffuse_directional = Diffuse(dirLightColor, dirLightIntensity, att_directional, -dirLightDirection, viewNormal);
    // specular reflected
    float3 specularReflected_directional = Speculate(
        dirLightColor * dirLightIntensity * specularColor, specularWeight, viewNormal,
        -dirLightDirection, viewFragPos, att_directional, specularGloss
    );
    // scale by shadow level
    diffuse_directional *= shadowLevel_directional;
    specularReflected_directional *= shadowLevel_spot;
    // ========================================================= DIRECTIONAL LIGHT ==================================================
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + diffuse_spot + diffuse_directional + ambient) * tex.Sample(splr, tc).rgb + specularReflected + specularReflected_spot + specularReflected_directional), 1.0f);
}