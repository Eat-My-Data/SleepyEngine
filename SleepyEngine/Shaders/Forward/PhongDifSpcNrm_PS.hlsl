#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/DirectionalLight.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/DefaultLightSettings.hlsl"
#include "../Common/SpotLight.hlsl"
#include "../Common/CameraData.hlsl"

cbuffer ObjectCBuf
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;

SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
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
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }
    
    // directional light
    float dirLightShadow = 1.0f; //CalculateDirectionalLightShadow(lightViewPos, splr);
    float dirLightAtt = directionalLightData[0].att;
    const float3 directionalDiffuse = Diffuse(directionalLightData[0].color.rgb, defaultLightIntensity,
        dirLightAtt, -directionalLightData[0].lightDirection, viewNormal) * dirLightShadow;
    const float3 camToFrag = viewFragPos - camPos.xyz;
    const float3 directionalSpecular = Speculate(
        directionalLightData[0].color.rgb, defaultLightIntensity, viewNormal, -directionalLightData[0].lightDirection,
        camToFrag, dirLightAtt, defaultSpecularPower
    );
    
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
    // specular parameter determination (mapped or uniform)
    float3 specularReflectionColor;
    float specularPower = specularGloss;
    const float4 specularSample = spec.Sample(splr, tc);
    if (useSpecularMap)
    {
        specularReflectionColor = specularSample.rgb;
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    if (useGlossAlpha)
    {
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }

	// attenuation
    float pointLightAtt = saturate((1 - (lv.distToL / pointLightData[0].radius)));
    pointLightAtt *= pointLightAtt; 
    // diffuse light
    const float3 pointDiffuse = Diffuse(pointLightData[0].color, defaultLightIntensity, pointLightAtt, lv.dirToL, viewNormal);
    // TODO: Figure out why using specularGloss instead of defaultSpecularPower breaks
    // specular reflected
    const float3 pointSpecular = Speculate(
        pointLightData[0].color * defaultLightIntensity * specularReflectionColor, specularWeight, viewNormal,
        lv.vToL, viewFragPos, pointLightAtt, defaultSpecularPower
    );
    
    // spot light
    float3 spotToFrag = spotLightData[0].pos - viewFragPos;
    float att = saturate((1 - (length(spotToFrag) / spotLightData[0].range)));
    att *= att;
    float angularAttFactor = max(0.0f, dot(normalize(-spotLightData[0].lightDirection), normalize(spotToFrag)));
    float3 spotDiffuse = float3(0.0f, 0.0f, 0.0f);
    float3 spotSpecular = float3(0.0f, 0.0f, 0.0f);
    if (angularAttFactor > spotLightData[0].outerRadius && att > 0.0f)
    {
        float conAtt = saturate((angularAttFactor - spotLightData[0].outerRadius) / (spotLightData[0].innerRadius - spotLightData[0].outerRadius));
        
        //float4 fragPositionInLightView = mul(float4(viewFragPos, 1.0f), spotLightData[0].spotViewProjectionMatrix);
        float spotLightShadow = 1.0f; //CalculateSpotLightShadow(fragPositionInLightView, splr);
        spotDiffuse = Diffuse(spotLightData[0].color.rgb, defaultLightIntensity, att * conAtt, -normalize(spotLightData[0].lightDirection), viewNormal) * spotLightShadow;
        spotSpecular = Speculate(
            spotLightData[0].color.rgb, defaultLightIntensity, viewNormal, -normalize(spotLightData[0].lightDirection),
            camToFrag, att * conAtt, defaultSpecularPower
        ) * spotLightShadow;
    }
    
    float3 combinedColor = directionalDiffuse + pointDiffuse + spotDiffuse + directionalSpecular + pointSpecular + spotSpecular + defaultAmbientLight;
	// final color
    return float4((combinedColor * tex.Sample(splr, tc).rgb), 1.0f);
}