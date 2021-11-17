#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/DefaultLightSettings.hlsl"

cbuffer ObjectCBuf
{
    float4 materialColor;
    float4 specularColor;
    float specularPower;
};


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
	// attenuation
    float pointLightAtt = saturate((1 - (lv.distToL / pointLightData[index].radius)));
    pointLightAtt *= pointLightAtt;
	// diffuse
    const float3 diffuse = Diffuse(pointLightData[0].color, defaultLightIntensity, pointLightAtt, lv.dirToL, viewNormal);
    // specular
    const float3 specular = Speculate(
        specularColor.rgb, 1.0f, viewNormal,
        lv.vToL, viewFragPos, pointLightAtt, specularPower
    );
	// final color
    return float4(saturate((diffuse + defaultAmbientLight) * materialColor.rgb + specular), 1.0f);
}