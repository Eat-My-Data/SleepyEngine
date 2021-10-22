#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/PointLight.hlsl"
#include "../Common/CameraData.hlsl"
#include "../Common/DefaultLightSettings.hlsl"

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D depthTexture : register(t3);

SamplerState SampleTypePoint : register(s0);

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
    // screen position
    float2 screenPos;
    screenPos.x = position.x / 1280.0f;
    screenPos.y = position.y / 720.0f;
    
    // sample textures
    float4 colors = colorTexture.Load(int3(position.xy, 0));
    float4 normals = normalTexture.Load(int3(position.xy, 0));
    float4 specular = specularTexture.Load(int3(position.xy, 0));
    float depthSample = depthTexture.Load(int3(position.xy, 0)).r;
    
    // clip space, negate y because directx
    float clipX = (screenPos.x * 2.0) - 1.0;
    float clipY = (screenPos.y * 2.0) - 1.0;
    clipY = -clipY;
   
    // normal to clip space
    normals = (normals * 2.0) - 1.0;

    // world position
    float4 worldDepth = float4(clipX, clipY, depthSample, 1.0);
    float4 worldPosition = mul(worldDepth, projInvMatrix);
    worldPosition /= worldPosition.w;
    float4 worldSpacePos = mul(worldPosition, viewInvMatrix);
    
    PointLightData pl = pointLightData[0];
    // light
    const float3 pointToFrag  = pointLightData[0].pos - worldSpacePos.xyz;
    float closestDepth = pointLightShadowTexture.Sample(SampleTypePoint, normalize(pointToFrag)).r;
    
    float shadow = CalculatePointLightShadow(worldSpacePos.xyz, pointLightData[0].pos, SampleTypePoint, 0);
    // vector from camera to fragment
    float3 camToFrag = worldSpacePos.xyz - camPos.xyz;
    
    float att = saturate((1 - (length(pointToFrag) / pointLightData[0].radius)));
    att *= att;

    // diffuse
    float3 diffuseColor = Diffuse(pointLightData[0].color, defaultLightIntensity, att, normalize(pointToFrag), normalize(normals.xyz)) * shadow;
    
    // specular
    float3 specularResult = Speculate(specular.xyz, defaultLightIntensity, normalize(normals.xyz), normalize(pointToFrag), camToFrag, att, defaultSpecularPower) * shadow;
    float3 combinedColor = ((diffuseColor + specularResult));

    // final color
    return float4((combinedColor * colors.rgb), 1.0f);
}