#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
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


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float4 lightViewPos : Position2) : SV_Target
{    
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
    // replace normal with mapped if normal mapping enabled
    if (normalMapEnabled)
    {
        viewNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
    }
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
	// attenuation
    const float att = Attenuate(pointLightData[0].attConst, pointLightData[0].attLin, pointLightData[0].attQuad, lv.distToL);
	// diffuse
    const float3 diffuse = Diffuse(pointLightData[0].color, pointLightData[0].diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular
    //const float3 specular = Speculate(
    //   pointLightData[0].color, pointLightData[0].diffuseIntensity, viewNormal,
    //    lv.vToL, viewFragPos, att, specularPower
    //);
    const float3 specular = { 0.0f, 0.0f, 0.0f };
     // fragment to light vector data
    const LightVectorData directionalLV = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
	// attenuation
    DirectionalLightData dl = directionalLightData[0];
    const float directionalAtt = directionalLightData[0].att;
	// diffuse intensity
    const float3 directionalDiffuse = Diffuse(pointLightData[0].color, dl.specularIntensity, directionalAtt, -dl.lightDirection, viewNormal);
	// specular
    const float3 directionalSpecular = Speculate(
        specularPower.rrr, 1.0f, viewNormal, -directionalLightData[0].lightDirection,
        viewFragPos, directionalAtt, specularPower
    );
    
    float fragDepth = lightViewPos.z / lightViewPos.w;
    float sampleDepth = depthTextureFromLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
    float isInLight = sampleDepth > fragDepth;
    float3 pl = pointLightData[0].ambient;
    float3 combinedColor = diffuse + specular + ((directionalDiffuse + directionalSpecular) * isInLight) + pl;
    
   	// final color
    return float4((combinedColor * tex.Sample(splr, tc).rgb), 1.0f);
}