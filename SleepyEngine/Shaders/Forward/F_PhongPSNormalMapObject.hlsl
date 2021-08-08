#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/Transform.hlsl"
#include "../Common/DirectionalLight.hlsl"

cbuffer ObjectCBuf
{
	float specularIntensity;
	float specularPower;
	bool normalMapEnabled;
	float padding[1];
};

Texture2D tex;
Texture2D nmap : register(t2);
Texture2D depthTextureFromLight : register(t4);
SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 lightViewPos : Position2) : SV_Target
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
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
	// attenuation
    const float att = Attenuate(pointLightData[0].attConst, pointLightData[0].attLin, pointLightData[0].attQuad, lv.distToL);
	// diffuse intensity
    const float3 diffuse = Diffuse(pointLightData[0].color, pointLightData[0].diffuseIntensity, att, lv.dirToL, viewNormal);
	// specular
    const float3 specular = Speculate(
        specularIntensity.rrr, 1.0f, viewNormal, lv.vToL,
        viewFragPos, att, specularPower2
    );
    
    // fragment to light vector data
    const LightVectorData directionalLV = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
	// attenuation
    const float directionalAtt = 0.8f;
	// diffuse intensity
    const float3 directionalDiffuse = Diffuse(pointLightData[0].color, pointLightData[0].diffuseIntensity, directionalAtt, -directionalLightData[0].lightDirection, viewNormal);
	// specular
    const float3 directionalSpecular = Speculate(
        specularIntensity.rrr, 1.0f, viewNormal, -directionalLightData[0].lightDirection,
        viewFragPos, directionalAtt, specularPower2
    );
    
    float fragDepth = lightViewPos.z / lightViewPos.w;
    float sampleDepth = depthTextureFromLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;    
    float isInLight = sampleDepth > fragDepth;
    float3 combinedColor = diffuse + specular + ((directionalDiffuse + directionalSpecular) * isInLight) + pointLightData[0].ambient;
    
   	// final color
    return float4((combinedColor * tex.Sample(splr, tc).rgb), 1.0f);
} 