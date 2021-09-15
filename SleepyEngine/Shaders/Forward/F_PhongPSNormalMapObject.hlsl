#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/Transform.hlsl"
#include "../Common/DirectionalLight.hlsl"
#include "../Common/SpotLight.hlsl"

cbuffer ObjectCBuf
{
	float specularIntensity;
	float specularPower;
	bool normalMapEnabled;
	float padding2[1];
};

Texture2D tex;
Texture2D nmap : register(t2);
SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 spotLightViewPos : Position2, float4 lightViewPos : Position2) : SV_Target
{
	// sample normal from map if normal mapping enabled
    if (normalMapEnabled)
    {
		// sample and unpack normal data
        const float3 normalSample = nmap.Sample(splr, tc).xyz;
        const float3 objectNormal = normalSample * 2.0f - 1.0f;
      	// bring normal from object space into view space
        viewNormal = normalize(mul(objectNormal, (float3x3) modelView));
    }
    const float specularPower2 = pointLightData[0].specularPower;

    
    float3 combinedPointLightDiffuse;
    float3 combinedPointLightSpecular;
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
        float3 localSpecular = Speculate(pointLightData[i].color, pointLightData[i].diffuseIntensity, viewNormal, lv.dirToL, viewFragPos, att, specularPower2);
        combinedPointLightSpecular += localSpecular * shadow;
    }
    
    float dirLightShadow = CalculateDirectionalLightShadow(lightViewPos, splr);

	// attenuation
    const float directionalAtt = 0.8f;
	// diffuse intensity
    const float3 directionalDiffuse = Diffuse(pointLightData[0].color, pointLightData[0].diffuseIntensity, directionalAtt, 
    -directionalLightData[0].lightDirection, viewNormal) * dirLightShadow;
	// specular
    const float3 camToFrag = viewFragPos - pointLightData[0].camPos;
    const float3 directionalSpecular = Speculate(
         directionalLightData[0].specularIntensity.rrr, 1.0f, viewNormal, -directionalLightData[0].lightDirection,
        camToFrag, directionalAtt, specularPower
    );
    
        // spot light
    float3 spotToFrag = spotLightData[0].pos - viewFragPos;
    float att = saturate((1 - (length(spotToFrag) / spotLightData[0].range)));
    att *= att;
    
    float spotLightShadow = CalculateSpotLightShadow(spotLightViewPos, splr);
    float3 spotDiffuse = Diffuse(spotLightData[0].color.rgb, 1.0f, att, -normalize(spotLightData[0].lightDirection), viewNormal);
    float3 combinedColor = combinedPointLightDiffuse + combinedPointLightSpecular + directionalDiffuse + directionalSpecular + spotDiffuse + pointLightData[0].ambient;
    
   	// final color
    return float4((combinedColor * tex.Sample(splr, tc).rgb), 1.0f);
} 