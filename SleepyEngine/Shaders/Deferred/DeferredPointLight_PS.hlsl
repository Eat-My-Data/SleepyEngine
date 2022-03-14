#include "../Common/ShaderOps.hlsli"
#include "../Common/LightVectorData.hlsli"
#include "../Common/PointLight.hlsli"
#include "../Common/Camera.hlsli"
#include "../Common/PShadow.hlsli"

Texture2D colorTexture : register(t4);
Texture2D normalTexture : register(t5);
Texture2D specularTexture : register(t6);
Texture2D depthTexture : register(t7);

TextureCube shadowMap : register(t3); // shadow map

SamplerState SampleTypePoint : register(s0);

float4 main(float4 position : SV_POSITION, float4 spos : ShadowPosition) : SV_TARGET
{   
    // sample textures
    float4 color = colorTexture.Load(int3(position.xy, 0));
    float4 normal = normalTexture.Load(int3(position.xy, 0));
    float4 specular = specularTexture.Load(int3(position.xy, 0));
    float depthSample = depthTexture.Load(int3(position.xy, 0)).r;
    
    // screen position
    float2 screenPos;
    screenPos.x = position.x / 1280.0f;
    screenPos.y = position.y / 720.0f;
    
    // clip space, negate y because directx
    float clipX = (screenPos.x * 2.0) - 1.0;
    float clipY = (screenPos.y * 2.0) - 1.0;
    clipY = -clipY;
   
    // normal to clip space
    normal = (normal * 2.0) - 1.0;

    //// world position
    float4 worldSpacePos = CalculateWorldPosition(float4(clipX, clipY, depthSample, 1.0));

    // vector from camera to fragment
    float3 camToFrag = worldSpacePos.xyz - camPos.xyz;

    // shadow
    const float shadow = Shadow(spos);
    
    // attenuation
    const float3 pointToFrag = viewLightPos - worldSpacePos.xyz;
    float att = saturate((1 - (length(pointToFrag) / radius)));
    att *= att;

    // lighting calculations
    float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, normalize(pointToFrag), normalize(normal.xyz)) * shadow;
    float3 specularReflected = Speculate(specular.xyz, diffuseIntensity, normalize(normal.xyz), normalize(pointToFrag), camToFrag, att, 128.0f) * shadow;
    
    // combined color
    float3 combinedColor = ((ambient + diffuse + specularReflected));

    // final color
    return float4((combinedColor * color.rgb), 1.0f);
}