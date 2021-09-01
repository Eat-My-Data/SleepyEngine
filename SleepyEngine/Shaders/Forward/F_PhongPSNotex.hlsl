#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/DirectionalLight.hlsl"

cbuffer ObjectCBuf
{
    float4 materialColor;
    float4 specularColor;
    float specularPower;
};

Texture2D depthTextureFromLight : register(t4);
SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float4 lightViewPos : SV_Position) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
	// fragment to light vector data
    float specularPower2 = pointLightData[0].specularPower;
    float3 combinedPointLightDiffuse;
    float3 combinedPointLightSpecular = { 0.0f, 0.0f, 0.0f };
    
    for (float i = 0; i < 2; i++)
    {
        // currently getting depth fine with vector from light to fragment
        // that depth comes in 0 to 1
        // how do i get depth from light
      
        // get vector between fragment position and light position
        float3 fragToLight = viewFragPos - pointLightData[i].pos;
        // use the light to fragment vector to sample from the depth map    
        float closestDepth = pointLightShadowTexture.Sample(splr, fragToLight).r;
        // it is currently in linear range between [0,1]. Re-transform back to original value
        closestDepth *= 25;
        // now get current linear depth as the length between the fragment and light position
        float currentDepth = length(fragToLight);
        // now test for shadows
        float bias = 0.05;
        float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
        
        
        // fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(pointLightData[i].pos, viewFragPos);
        float att = saturate((1 - (lv.distToL / pointLightData[i].radius)));
        att *= att;
	    // diffuse
        float3 localDiffuse = Diffuse(pointLightData[i].color, pointLightData[i].diffuseIntensity, att, lv.dirToL, viewNormal);
        combinedPointLightDiffuse += localDiffuse * shadow;
	    // specular
        float3 localSpecular = Speculate(pointLightData[i].color, pointLightData[i].diffuseIntensity, viewNormal, lv.vToL, viewFragPos, att, specularPower);
        combinedPointLightSpecular += localSpecular * shadow;
    }
    
	// attenuation
    const float directionalAtt = 0.8f;
	// diffuse intensity
    const float3 directionalDiffuse = Diffuse(pointLightData[0].color, pointLightData[0].diffuseIntensity, directionalAtt, -directionalLightData[0].lightDirection, viewNormal);
	// specular
    const float3 directionalSpecular = Speculate(
        specularPower.rrr, 1.0f, viewNormal, -directionalLightData[0].lightDirection,
        viewFragPos, directionalAtt, specularPower2
    );

    float fragDepth = lightViewPos.z / lightViewPos.w;
    float sampleDepth = depthTextureFromLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
    
    float isInLight = sampleDepth > fragDepth;
    float3 combinedColor = combinedPointLightDiffuse + combinedPointLightSpecular + ((directionalDiffuse + directionalSpecular) * isInLight) + pointLightData[0].ambient;

   	// final color
    return float4(combinedColor, 1.0f) * materialColor;
}