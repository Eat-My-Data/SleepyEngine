#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
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


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float4 lightViewPos : Position2) : SV_Target
{    
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
    // replace normal with mapped if normal mapping enabled
    if (normalMapEnabled)
    {
        viewNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
    }
    float specularPower = pointLightData[0].specularPower;
    
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
    
    float dirLightShadow = CalculateDirectionalLightShadow(lightViewPos, splr);

	// attenuation
    DirectionalLightData dl = directionalLightData[0];
    const float3 camToFrag = viewFragPos - pointLightData[0].camPos;
    const float directionalAtt = directionalLightData[0].att;
	// diffuse intensity
    const float3 directionalDiffuse = Diffuse(pointLightData[0].color, dl.specularIntensity, directionalAtt, -dl.lightDirection, viewNormal) * dirLightShadow;
	// specular
    const float3 directionalSpecular = Speculate(
        directionalLightData[0].specularIntensity.rrr, 1.0f, viewNormal, -directionalLightData[0].lightDirection,
        camToFrag, directionalAtt, specularPower
    ) * dirLightShadow;

    // spot light
    float3 spotToFrag = spotLightData[0].pos - viewFragPos;
    float att = saturate((1 - (length(spotToFrag) / spotLightData[0].range)));
    att *= att;
    float3 spotDiffuse = Diffuse(spotLightData[0].color.rgb, 1.0f, att, -normalize(spotLightData[0].lightDirection), viewNormal);
    
    float3 pl = pointLightData[0].ambient;
    float3 combinedColor = combinedPointLightDiffuse + combinedPointLightSpecular + directionalDiffuse + directionalSpecular + pl + spotDiffuse;
    
   	// final color
    return float4((combinedColor * tex.Sample(splr, tc).rgb), 1.0f);
}