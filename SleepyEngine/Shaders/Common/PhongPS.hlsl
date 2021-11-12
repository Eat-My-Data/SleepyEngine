#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"
#include "DefaultLightSettings.hlsl"

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    float padding3[2];
};

Texture2D tex;

SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    // renormalize interpolated normal
    viewNormal = normalize(viewNormal);
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
	// attenuation
    float pointLightAtt = saturate((1 - (lv.distToL / pointLightData[index].radius)));
    pointLightAtt *= pointLightAtt;
    
    float3 pointLightDiffuse = Diffuse(pointLightData[0].color, defaultLightIntensity, pointLightAtt, lv.dirToL, viewNormal);
    float3 pointLightSpecular = Speculate(pointLightData[0].color, defaultLightIntensity, viewNormal, lv.dirToL, viewFragPos, pointLightAtt, defaultSpecularPower);
	// final color
    return float4(saturate((pointLightDiffuse + pointLightSpecular + defaultAmbientLight) * tex.Sample(splr, tc).rgb), 1.0f);
}