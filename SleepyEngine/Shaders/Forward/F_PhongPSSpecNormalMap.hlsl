#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/DirectionalLight.hlsl"

cbuffer ObjectCBuf
{
    bool normalMapEnabled;
    bool specularMapEnabled;
    bool hasGloss;
    float specularPowerConst;
    float3 specularColor;
    float specularMapWeight;
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;
Texture2D depthTextureFromLight : register(t4);

SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float4 lightViewPos : Position2) : SV_Target
{
    // sample diffuse texture 
    float4 dtex = tex.Sample(splr, tc);
    
    #ifdef MASK_BOI
    // bail if highly translucent
    clip(dtex.a < 0.1f ? -1 : 1);
    // flip normal when backface
    if (dot(viewNormal, viewFragPos) >= 0.0f)
    {
        viewNormal = -viewNormal;
    }
    #endif
    
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
    // replace normal with mapped if normal mapping enabled
    if (normalMapEnabled)
    {
        viewNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
    }
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
    // specular parameter determination (mapped or uniform)
    float3 specularReflectionColor;
    float specularPower = specularPowerConst;
    if (specularMapEnabled)
    {
        const float4 specularSample = spec.Sample(splr, tc);
        specularReflectionColor = specularSample.rgb * specularMapWeight;
        if (hasGloss)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
    else
    {
        specularReflectionColor = specularColor;
    }
	// attenuation
    PointLightData pl = pointLightData[0];
	// diffuse light
    float3 combinedPointLightDiffuse;
    float3 combinedPointLightSpecular;
    //float specularPower = directionalLightData[0].specularPower;
    
    for (float i = 0; i < 2; i++)
    {
        // fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
	    // attenuation
        const float att = pl.attConst; //Attenuate(pointLightData[0].attConst, pointLightData[0].attLin, pointLightData[0].attQuad, lv.distToL);
	    // diffuse
        combinedPointLightDiffuse += Diffuse(pointLightData[0].color, pointLightData[0].diffuseIntensity, att, lv.dirToL, viewNormal);
	    // specular
        combinedPointLightSpecular += Speculate(pointLightData[0].color, pointLightData[0].diffuseIntensity, viewNormal, lv.vToL, viewFragPos, att, specularPower);
    }
    
    // fragment to light vector data
    const LightVectorData directionalLV = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
	// attenuation
    const float directionalAtt = 0.8f;
	// diffuse intensity
    const float3 directionalDiffuse = Diffuse(pointLightData[0].color, pointLightData[0].diffuseIntensity, directionalAtt, -directionalLightData[0].lightDirection, viewNormal);
	// specular
    //const float3 directionalSpecular = Speculate(
    //    specularPower.rrr, 1.0f, viewNormal, -directionalLightData[0].lightDirection,
    //    viewFragPos, directionalAtt, specularPower
    //);
    const float3 directionalSpecular = { 0.0f, 0.0f, 0.0f };

    
    float fragDepth = lightViewPos.z / lightViewPos.w;
    float sampleDepth = depthTextureFromLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
    float isInLight = sampleDepth > fragDepth;
    float3 combinedColor = combinedPointLightDiffuse + combinedPointLightSpecular + ((directionalDiffuse + directionalSpecular) * isInLight) + pointLightData[0].ambient;
    
   	// final color
    return float4((combinedColor * tex.Sample(splr, tc).rgb), 1.0f);
}