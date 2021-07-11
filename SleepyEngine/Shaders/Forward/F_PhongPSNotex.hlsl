#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"

cbuffer ObjectCBuf
{
    float4 materialColor;
    float4 specularColor;
    float specularPower;
};

cbuffer ObjectCBuf
{
    float3 lightDirection;
    float padding2[1];
}

Texture2D depthTextureFromLight : register(t4);
SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float4 lightViewPos : SV_Position) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular
    const float3 specular = Speculate(
        specularColor.rgb, 1.0f, viewNormal,
        lv.vToL, viewFragPos, att, specularPower
    );
    
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
    
    float3 combinedDiffuse = (diffuse + directionalDiffuse) / 2.0f;
    float3 combinedSpecular = (specular + directionalSpecular) / 2.0f;
    
    float fragDepth = lightViewPos.z / lightViewPos.w;
    float sampleDepth = depthTextureFromLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
    
    if (sampleDepth < fragDepth)
    {
        // placeholder shadow
        return float4(saturate((combinedDiffuse + ambient) * materialColor.rgb + combinedSpecular), 1.0f) * float4(.4, .4, .4, 1.0);
    }
    
	// final color
    return float4(saturate((combinedDiffuse + ambient) * materialColor.rgb + combinedSpecular), 1.0f);
}