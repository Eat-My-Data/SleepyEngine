#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/DefaultLightSettings.hlsl"

cbuffer ObjectCBuf
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex;
Texture2D nmap : register(t2);

SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
    // replace normal with mapped if normal mapping enabled
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
	// attenuation
    float pointLightAtt = saturate((1 - (lv.distToL / pointLightData[index].radius)));
    pointLightAtt *= pointLightAtt;
	// diffuse
    const float3 diffuse = Diffuse(pointLightData[0].color, defaultLightIntensity, pointLightAtt, lv.dirToL, viewNormal);
    // TODO: Figure out why using specularGloss instead of defaultSpecularPower breaks
    // specular
    const float3 specular = Speculate(
        pointLightData[0].color * defaultLightIntensity * specularColor, specularWeight, viewNormal,
        lv.vToL, viewFragPos, pointLightAtt, defaultSpecularPower
    );
	// final color
    return float4(saturate((diffuse + defaultAmbientLight) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}