#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/DirectionalLight.hlsl"
#include "../Common/SpotLight.hlsl"

cbuffer ObjectCBuf
{
    float4 materialColor;
    float4 specularColor;
    float specularPower;
};

SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float4 spotLightViewPos : Position2, float4 lightViewPos : SV_Position) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
	// fragment to light vector data
    float specularPower2 = pointLightData[0].specularPower;
    float3 combinedPointLightDiffuse;
    float3 combinedPointLightSpecular = { 0.0f, 0.0f, 0.0f };
    
    for (float i = 0; i < numPointLights; i++)
    {
        float shadow = CalculatePointLightShadow(viewFragPos, pointLightData[i].pos, splr, i);
        
        // fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(pointLightData[i].pos, viewFragPos);
        float att = saturate((1 - (lv.distToL / pointLightData[i].radius)));
        att *= att;
	    // diffuse
        float3 localDiffuse = Diffuse(pointLightData[i].color, pointLightData[i].diffuseIntensity, att, lv.dirToL, viewNormal);
        combinedPointLightDiffuse += localDiffuse * shadow;
	    // specular
        float3 localSpecular = Speculate(pointLightData[i].color, pointLightData[i].diffuseIntensity, viewNormal, lv.dirToL, viewFragPos, att, specularPower);
        combinedPointLightSpecular += localSpecular * shadow;
    }
    
    float dirLightShadow = CalculateDirectionalLightShadow(lightViewPos, splr);

	// attenuation
    const float directionalAtt = 0.8f;
	// diffuse intensity
    const float3 directionalDiffuse = Diffuse(pointLightData[0].color, pointLightData[0].diffuseIntensity, 
    directionalAtt, -directionalLightData[0].lightDirection, viewNormal) * dirLightShadow;
	// specular
    const float3 camToFrag = viewFragPos - pointLightData[0].camPos;
    const float3 directionalSpecular = Speculate(
         directionalLightData[0].color.rgb, directionalLightData[0].specularIntensity, viewNormal, -directionalLightData[0].lightDirection,
        camToFrag, directionalAtt, specularPower
    );
        // spot light
    float3 spotToFrag = spotLightData[0].pos - viewFragPos;
    float att = saturate((1 - (length(spotToFrag) / spotLightData[0].range)));
    att *= att;
    
    float spotLightShadow = CalculateSpotLightShadow(spotLightViewPos, splr);
    float3 spotDiffuse = Diffuse(spotLightData[0].color.rgb, 1.0f, att, normalize(spotLightData[0].lightDirection), viewNormal) * spotLightShadow;
    const float3 spotSpecular = Speculate(
        spotLightData[0].color.rgb, directionalLightData[0].specularIntensity, viewNormal, normalize(spotLightData[0].lightDirection),
        camToFrag, att, specularPower
    ) * spotLightShadow;
    
    float3 combinedColor = combinedPointLightDiffuse + combinedPointLightSpecular + directionalDiffuse + directionalSpecular + spotDiffuse + spotSpecular + pointLightData[0].ambient;

   	// final color
    return float4(combinedColor, 1.0f) * materialColor;
}