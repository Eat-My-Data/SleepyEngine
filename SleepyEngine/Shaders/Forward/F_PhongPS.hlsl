#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/DirectionalLight.hlsl"
#include "../Common/SpotLight.hlsl"
#include "../Common/CameraData.hlsl"
#include "../Common/DefaultLightSettings.hlsl"

Texture2D tex;
SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 spotLightViewPos : Position2, float4 lightViewPos : SV_Position) : SV_Target
{
	// renormalize interpolated normal
    viewNormal = normalize(viewNormal);
       
    // directional light
    float dirLightShadow = CalculateDirectionalLightShadow(lightViewPos, splr);
    float dirLightAtt = directionalLightData[0].att;
    const float3 directionalDiffuse = Diffuse(directionalLightData[0].color.rgb, defaultLightIntensity, 
        dirLightAtt, -directionalLightData[0].lightDirection, viewNormal) * dirLightShadow;
    const float3 camToFrag = viewFragPos - camPos.xyz;
    const float3 directionalSpecular = Speculate(
        directionalLightData[0].color.rgb, defaultLightIntensity, viewNormal, -directionalLightData[0].lightDirection,
        camToFrag, dirLightAtt, defaultSpecularPower
    );
    
    // point light
    float shadow = CalculatePointLightShadow(viewFragPos, pointLightData[0].pos, splr, 0);
    const LightVectorData lv = CalculateLightVectorData(pointLightData[0].pos, viewFragPos);
    float pointLightAtt = saturate((1 - (lv.distToL / pointLightData[0].radius)));
    pointLightAtt *= pointLightAtt;
    float3 pointLightDiffuse = Diffuse(pointLightData[0].color, defaultLightIntensity, pointLightAtt, lv.dirToL, viewNormal) * shadow;
    float3 pointLightSpecular = Speculate(pointLightData[0].color, defaultLightIntensity, viewNormal, lv.dirToL, viewFragPos, pointLightAtt, defaultSpecularPower) * shadow;
    
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
        
        float spotLightShadow = CalculateSpotLightShadow(spotLightViewPos, splr);
        spotDiffuse = Diffuse(spotLightData[0].color.rgb, defaultLightIntensity, att * conAtt, -normalize(spotLightData[0].lightDirection), viewNormal) * spotLightShadow;
        spotSpecular = Speculate(
            spotLightData[0].color.rgb, defaultLightIntensity, viewNormal, -normalize(spotLightData[0].lightDirection),
            camToFrag, att * conAtt, defaultSpecularPower
        ) * spotLightShadow;
    }
    
    float3 combinedColor = pointLightDiffuse + pointLightSpecular + directionalDiffuse + directionalSpecular + spotDiffuse + spotSpecular + defaultAmbientLight;
    
   	// final color
    return float4((combinedColor * tex.Sample(splr, tc).rgb), 1.0f);
}