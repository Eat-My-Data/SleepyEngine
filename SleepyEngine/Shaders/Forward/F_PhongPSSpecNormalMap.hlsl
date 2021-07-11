#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"

cbuffer ObjectCBuf
{
    bool normalMapEnabled;
    bool specularMapEnabled;
    bool hasGloss;
    float specularPowerConst;
    float3 specularColor;
    float specularMapWeight;
};

cbuffer ObjectCBuf
{
    float3 lightDirection;
    float padding2[1];
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
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
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
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse light
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular reflected
    const float3 specularReflected = Speculate(
        specularReflectionColor, 1.0f, viewNormal,
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
    float3 combinedSpecular = (specularReflected + directionalSpecular) / 2.0f;
    
    float fragDepth = lightViewPos.z / lightViewPos.w;
    float sampleDepth = depthTextureFromLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
    
    if (sampleDepth < fragDepth)
    {
        // placeholder shadow
        return float4(saturate((combinedDiffuse + ambient) * tex.Sample(splr, tc).rgb + combinedSpecular), 1.0f) * float4(.4, .4, .4, 1.0);
    }
    
    // final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((combinedDiffuse + ambient) * dtex.rgb + combinedSpecular), dtex.a);
}