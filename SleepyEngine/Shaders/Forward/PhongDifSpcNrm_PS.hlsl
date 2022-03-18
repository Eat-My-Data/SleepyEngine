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
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex : register(t0);
Texture2D spec : register(t1);
Texture2D nmap : register(t2);

SamplerState splr : register(s0);


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float4 spos : ShadowPosition) : SV_Target
{    
    // sample diffuse texture
    const float4 dtex = tex.Sample(splr, tc);
#ifdef MASK_BOI
    // bail if highly translucent
    clip(dtex.a < 0.1f ? -1 : 1);
    // flip normal when backface
    if (dot(viewNormal, viewFragPos) >= 0.0f)
    {
        viewNormal = -viewNormal;
    }
#endif
    // ========================================================= POINT LIGHT ========================================================
    // shadow map test
    const float shadowLevel = 1; //Shadow(spos);
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
    // replace normal with mapped if normal mapping enabled
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
    // specular parameter determination (mapped or uniform)
    float3 specularReflectionColor;
    float specularPower = specularGloss;
    const float4 specularSample = spec.Sample(splr, tc);
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
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse light
    float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular reflected
    float3 specularReflected = Speculate(
        diffuseColor * diffuseIntensity * specularReflectionColor, specularWeight, viewNormal,
        lv.vToL, viewFragPos, att, specularPower
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
        lv_spot.vToL, viewFragPos, att_spot, specularPower
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
    return float4(saturate((diffuse + diffuse_spot + diffuse_directional + ambient) * dtex.rgb + specularReflected + specularReflected_spot + specularReflected_directional), 1.0f);
}