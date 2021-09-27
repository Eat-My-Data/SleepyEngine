#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/SpotLight.hlsl"
#include "../Common/PointLight.hlsl"

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
    float4 worldPosition = mul(worldDepth, pointLightData[0].projInvMatrix);
    worldPosition /= worldPosition.w;
    float4 worldSpacePos = mul(worldPosition, pointLightData[0].cameraMatrix);
    
    PointLightData pl = pointLightData[index];
    // light
    const LightVectorData lv = CalculateLightVectorData(spotLightData[0].pos, worldSpacePos.xyz);
    float closestDepth = pointLightShadowTexture.Sample(SampleTypePoint, lv.vToL).r;
    
    float shadow = CalculatePointLightShadow(worldSpacePos.xyz, spotLightData[0].pos, SampleTypePoint, index);
    // vector from camera to fragment
    float3 camToFrag = worldSpacePos.xyz - pl.camPos;
    
    float att = saturate((1 - (lv.distToL / pl.radius)));
    att *= att;

    // diffuse
    float3 diffuseColor = Diffuse(pointLightData[index].color, pl.diffuseIntensity, att, lv.dirToL, normalize(normals.xyz)) * shadow;
    
    // specular
    float3 specularResult = Speculate(specular.xyz, pl.diffuseIntensity, normalize(normals.xyz), lv.dirToL, camToFrag, att, pl.specularPower) * shadow;
    float3 combinedColor = ((diffuseColor + specularResult));

    // final color
    return float4((combinedColor * colors.rgb), 1.0f);
}