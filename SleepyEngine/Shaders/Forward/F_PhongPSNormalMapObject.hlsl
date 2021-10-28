#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/Transform.hlsl"
#include "../Common/DirectionalLight.hlsl"
#include "../Common/SpotLight.hlsl"
#include "../Common/CameraData.hlsl"
#include "../Common/DefaultLightSettings.hlsl"

cbuffer ObjectCBuf
{
	float specularIntensity;
	float specularPower;
	bool normalMapEnabled;
	float padding2[1];
};

Texture2D tex;
Texture2D nmap : register(t2);
SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 spotLightViewPos : Position2, float4 lightViewPos : Position2) : SV_Target
{
	// sample normal from map if normal mapping enabled
    if (normalMapEnabled)
    {
		// sample and unpack normal data
        const float3 normalSample = nmap.Sample(splr, tc).xyz;
        const float3 objectNormal = normalSample * 2.0f - 1.0f;
      	// bring normal from object space into view space
        viewNormal = normalize(mul(objectNormal, (float3x3) modelView));
    }

    // camera to fragment vector
    const float3 camToFrag = viewFragPos - camPos.xyz;

    // directional light
    const float dirLightShadow = CalculateDirectionalLightShadow(lightViewPos, splr);
    const float dirLightAtt = directionalLightData[0].att;
    const float3 directionalDiffuse = Diffuse(directionalLightData[0].color.rgb, defaultLightIntensity,
        dirLightAtt, -directionalLightData[0].lightDirection, viewNormal) * dirLightShadow;
    const float3 directionalSpecular = Speculate(
        directionalLightData[0].color.rgb, defaultLightIntensity, viewNormal, -directionalLightData[0].lightDirection,
        camToFrag, dirLightAtt, specularPower) * dirLightShadow;
    
    // point light
    float3 pointLightDiffuse = { 0.0f, 0.0f, 0.0f };
    float3 pointLightSpecular = { 0.0f, 0.0f, 0.0f };
    for (int index = 0; index < numPointLights; index++)
    {
        float shadow = 1.0f; //CalculatePointLightShadow(viewFragPos, pointLightData[index].pos, splr, 0);
        const LightVectorData lv = CalculateLightVectorData(pointLightData[index].pos, viewFragPos);
        float pointLightAtt = saturate((1 - (lv.distToL / pointLightData[index].radius)));
        pointLightAtt *= pointLightAtt;
        pointLightDiffuse += Diffuse(pointLightData[index].color, defaultLightIntensity, pointLightAtt, lv.dirToL, viewNormal) * shadow;
        pointLightSpecular += Speculate(pointLightData[index].color, defaultLightIntensity, viewNormal, lv.dirToL, viewFragPos, pointLightAtt, defaultSpecularPower) * shadow;
    }
    
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
        
        float4 fragPositionInLightView = mul(float4(viewFragPos, 1.0f), spotLightData[0].spotViewProjectionMatrix);
        float spotLightShadow = CalculateSpotLightShadow(fragPositionInLightView, splr);
        spotDiffuse = Diffuse(spotLightData[0].color.rgb, defaultLightIntensity, spotLightAtt * conAtt,
            -normalize(spotLightData[0].lightDirection), viewNormal) * spotLightShadow;
        spotSpecular = Speculate(
            spotLightData[0].color.rgb, defaultLightIntensity, viewNormal, -normalize(spotLightData[0].lightDirection),
            camToFrag, spotLightAtt * conAtt, specularPower ) * spotLightShadow;
    }
    
    float3 combinedColor = pointLightDiffuse + pointLightSpecular + directionalDiffuse + directionalSpecular + spotDiffuse + spotSpecular + defaultAmbientLight;
    
   	// final color
    return float4((combinedColor * tex.Sample(splr, tc).rgb), 1.0f);
} 