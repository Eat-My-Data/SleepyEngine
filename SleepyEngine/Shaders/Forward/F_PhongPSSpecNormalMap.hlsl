#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/DirectionalLight.hlsl"
#include "../Common/SpotLight.hlsl"
#include "../Common/CameraData.hlsl"
#include "../Common/DefaultLightSettings.hlsl"

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

SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float4 spotLightViewPos : Position2, float4 lightViewPos : Position2) : SV_Target
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
    specularPower = defaultSpecularPower;
    
    // camera to fragment vector
    const float3 camToFrag = viewFragPos - camPos.xyz;

	// directional light
    float dirLightShadow = CalculateDirectionalLightShadow(lightViewPos, splr);
    float dirLightAtt = directionalLightData[0].att;
    const float3 directionalDiffuse = Diffuse(directionalLightData[0].color.rgb, defaultLightIntensity,
        dirLightAtt, -directionalLightData[0].lightDirection, viewNormal) * dirLightShadow;
    const float3 directionalSpecular = Speculate(
        directionalLightData[0].color.rgb, defaultLightIntensity, viewNormal, -directionalLightData[0].lightDirection,
        camToFrag, dirLightAtt, specularPower) * dirLightShadow;
    
    // point light
    const float shadow = CalculatePointLightShadow(viewFragPos, pointLightData[0].pos, splr, 0);
    const float3 pointToFrag = pointLightData[0].pos - viewFragPos;
    float pointLightAtt = saturate((1 - (length(pointToFrag) / pointLightData[0].radius)));
    pointLightAtt *= pointLightAtt;
    const float3 pointLightDiffuse = Diffuse(pointLightData[0].color, defaultLightIntensity, pointLightAtt, normalize(pointToFrag),
        viewNormal) * shadow;
    const float3 pointLightSpecular = Speculate(pointLightData[0].color, defaultLightIntensity, viewNormal, normalize(pointToFrag),
        viewFragPos, pointLightAtt, specularPower) * shadow;

    // spot light
    const float3 spotToFrag = spotLightData[0].pos - viewFragPos;
    float spotLightAtt = saturate((1 - (length(spotToFrag) / spotLightData[0].range)));
    spotLightAtt *= spotLightAtt;
    float angularAttFactor = max(0.0f, dot(normalize(-spotLightData[0].lightDirection), normalize(spotToFrag)));
    float3 spotDiffuse = float3(0.0f, 0.0f, 0.0f);
    float3 spotSpecular = float3(0.0f, 0.0f, 0.0f);
    if (angularAttFactor > spotLightData[0].outerRadius && spotLightAtt > 0.0f)
    {
        float conAtt = saturate((angularAttFactor - spotLightData[0].outerRadius) / (spotLightData[0].innerRadius - spotLightData[0].outerRadius));
        
        float spotLightShadow = CalculateSpotLightShadow(spotLightViewPos, splr);
        spotDiffuse = Diffuse(spotLightData[0].color.rgb, defaultLightIntensity, spotLightAtt * conAtt,
            -normalize(spotLightData[0].lightDirection), viewNormal) * spotLightShadow;
        spotSpecular = Speculate(
            spotLightData[0].color.rgb, defaultLightIntensity, viewNormal, -normalize(spotLightData[0].lightDirection),
            camToFrag, spotLightAtt * conAtt, specularPower) * spotLightShadow;
    }
    
    float3 combinedColor = pointLightDiffuse + pointLightSpecular + directionalDiffuse + directionalSpecular + spotDiffuse + spotSpecular + defaultAmbientLight;
    
   	// final color
    return float4((combinedColor * tex.Sample(splr, tc).rgb), 1.0f);
}