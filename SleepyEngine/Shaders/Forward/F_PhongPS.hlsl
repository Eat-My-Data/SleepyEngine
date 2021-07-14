#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    float padding[2];
};

cbuffer DirectionalLight
{
    float3 lightDirection;
    float padding2[1];
}

Texture2D tex;
Texture2D depthTextureFromLight : register(t4);
SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 lightViewPos : SV_Position) : SV_Target
{
    
	// renormalize interpolated normal
    viewNormal = normalize(viewNormal);
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
	// specular
    const float3 specular = Speculate(diffuseColor, diffuseIntensity, viewNormal, lv.vToL, viewFragPos, att, specularPower);
    
     // fragment to light vector data
    const LightVectorData directionalLV = CalculateLightVectorData(viewLightPos, viewFragPos);
	// attenuation
    const float directionalAtt = 0.8f;
	// diffuse intensity
    const float3 directionalDiffuse = Diffuse(diffuseColor, diffuseIntensity, directionalAtt, -lightDirection, viewNormal);
	// specular
    const float3 directionalSpecular = Speculate(
        specularPower.rrr, 1.0f, viewNormal, -lightDirection,
        viewFragPos, directionalAtt, specularPower
    );
    
    float fragDepth = lightViewPos.z / lightViewPos.w;
    float sampleDepth = depthTextureFromLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
    float isInLight = sampleDepth > fragDepth;
    float3 combinedColor = diffuse + specular + ((directionalDiffuse + directionalSpecular) * isInLight) + ambient;
    
   	// final color
    return float4((combinedColor * tex.Sample(splr, tc).rgb), 1.0f);
}