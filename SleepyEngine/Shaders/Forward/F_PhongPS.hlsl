#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/DirectionalLight.hlsl"

Texture2D tex;
Texture2D depthTextureFromLight : register(t4);
SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 lightViewPos : SV_Position) : SV_Target
{
    
	// renormalize interpolated normal
    viewNormal = normalize(viewNormal);
    
    float3 combinedPointLightDiffuse;
    float3 combinedPointLightSpecular;
    float specularPower = directionalLightData[0].specularPower;
    
    for (float i = 0; i < 2; i++)
    {
        // fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
	    // attenuation
        const float att = Attenuate(pointLightData[i].attConst, pointLightData[0].attLin, pointLightData[i].attQuad, lv.distToL);
	    // diffuse
        combinedPointLightDiffuse += Diffuse(pointLightData[i].color, pointLightData[i].diffuseIntensity, att, lv.dirToL, viewNormal);
	    // specular
        combinedPointLightSpecular += Speculate(pointLightData[i].color, pointLightData[i].diffuseIntensity, viewNormal, lv.vToL, viewFragPos, att, specularPower);
    }
    
    // fragment to light vector data    
    const LightVectorData directionalLV = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
	// attenuation
    const float directionalAtt = 0.8f;
	// diffuse intensity
    const float3 directionalDiffuse = Diffuse(directionalLightData[0].color.rgb, pointLightData[0].diffuseIntensity, directionalAtt, -directionalLightData[0].lightDirection, viewNormal);
	// specular
    const float3 directionalSpecular = Speculate(
        specularPower.rrr, 1.0f, viewNormal, -directionalLightData[0].lightDirection,
        viewFragPos, directionalAtt, specularPower
    );
    
    float fragDepth = lightViewPos.z / lightViewPos.w;
    float sampleDepth = depthTextureFromLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
    float isInLight = sampleDepth > fragDepth;
    float3 combinedColor = combinedPointLightDiffuse + combinedPointLightSpecular + ((directionalDiffuse + directionalSpecular) * isInLight) + pointLightData[0].ambient;
    
   	// final color
    return float4((combinedColor * tex.Sample(splr, tc).rgb), 1.0f);
}