#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/DirectionalLight.hlsl"
#include "../Common/SpotLight.hlsl"

Texture2D tex;
SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 lightViewPos : SV_Position) : SV_Target
{
    
	// renormalize interpolated normal
    viewNormal = normalize(viewNormal);
    
    float specularPower = directionalLightData[0].specularPower;
   
    float dirLightShadow = CalculateDirectionalLightShadow(lightViewPos, splr);

	// attenuation
    const float directionalAtt = 0.8f;
	// diffuse intensity
    const float3 directionalDiffuse = Diffuse(directionalLightData[0].color.rgb, pointLightData[0].diffuseIntensity, 
    directionalAtt, -directionalLightData[0].lightDirection, viewNormal) * dirLightShadow;
	// specular
    const float3 camToFrag = viewFragPos - pointLightData[0].camPos;
    const float3 directionalSpecular = Speculate(
         directionalLightData[0].specularIntensity.rrr, 1.0f, viewNormal, -directionalLightData[0].lightDirection,
        camToFrag, directionalAtt, specularPower
    );
    
    // point lights
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
        float3 localSpecular = Speculate(pointLightData[i].color, pointLightData[i].diffuseIntensity, viewNormal, lv.dirToL, viewFragPos, att, specularPower);
        combinedPointLightSpecular += localSpecular * shadow;
    }
    
    // spot light
    float3 spotToFrag = spotLightData[0].pos - viewFragPos;
    float att = saturate((1 - (length(spotToFrag) / spotLightData[0].range)));
    att *= att;
    
    float3 spotDiffuse = Diffuse(spotLightData[0].color.rgb, 1.0f, att, -normalize(spotLightData[0].lightDirection), viewNormal);
    float3 combinedColor = combinedPointLightDiffuse + combinedPointLightSpecular + directionalDiffuse + directionalSpecular + spotDiffuse + pointLightData[0].ambient;
    
   	// final color
    return float4((combinedColor * tex.Sample(splr, tc).rgb), 1.0f);
}