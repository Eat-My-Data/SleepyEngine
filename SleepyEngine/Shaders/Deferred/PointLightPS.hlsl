#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D depthTexture : register(t3);

SamplerState SampleTypePoint : register(s0);

struct PointLightData
{
    float3 pos;
    float3 ambient;
    float3 color;
    float specularPower;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
    float3 camPos;
    float radius;
    row_major float4x4 cameraMatrix;
    row_major float4x4 projInvMatrix;
};

StructuredBuffer<PointLightData> pointLightData : register(t6);


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
    
    PointLightData pl = pointLightData[0];
    // light
    const LightVectorData lv = CalculateLightVectorData( pointLightData[0].pos, worldSpacePos.xyz);
    
    // vector from camera to fragment
    float3 camToFrag = worldSpacePos.xyz - pl.camPos;
    
    // attenutation
    float att = Attenuate(pointLightData[0].attConst, pl.attLin, pointLightData[0].attQuad, lv.distToL);

    // diffuse
    float3 diffuseColor = Diffuse(colors.rgb, pl.diffuseIntensity, att, -lv.dirToL / pl.radius, normalize(normals.xyz));
    
    // specular
    float3 specularResult = Speculate(specular.xyz, pl.diffuseIntensity, normalize(normals.xyz), lv.dirToL / pl.radius, camToFrag, att, pl.specularPower);
    
    // final color
    return float4(pointLightData[0].color, 1) * (float4(diffuseColor, 1) + float4(specularResult, 1.0f));
    
}