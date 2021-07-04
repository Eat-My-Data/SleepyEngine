#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D depthTexture : register(t3);

SamplerState SampleTypePoint : register(s0);

cbuffer CBuf : register(b0)
{
    float3 color;
    float radius;
    
    float attConst;
    float attLin;
    float attQuad;
    float specularPower;
    
    float diffuseIntensity;
    float specularIntensity;
    
    row_major float4x4 cameraMatrix;
    row_major float4x4 projInvMatrix;
};

cbuffer CBuf : register(b1)
{
    float3 lightPosition;
    float padding2;
};

cbuffer CamPosBuffer : register(b2)
{
    float3 camPos;
    float padding3;
};

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
    float4 worldSpacePos = mul(worldPosition, cameraMatrix);
    
    // light
    const LightVectorData lv = CalculateLightVectorData(lightPosition, worldSpacePos.xyz);
    
    // vector from camera to fragment
    float3 camToFrag = worldSpacePos.xyz - camPos;
    
    // attenutation
    float att = Attenuate(attConst, attLin, attQuad, lv.distToL);

    // diffuse
    float3 diffuseColor = Diffuse(colors.rgb, diffuseIntensity, att, lv.dirToL / radius, normalize(normals.xyz));
    
    // specular
    float3 specularResult = Speculate(specular.xyz, specularIntensity, normalize(normals.xyz), lv.dirToL / radius, camToFrag, att, specularPower);
    
    // final color
    return float4(color, 1) * (float4(diffuseColor, 1) + float4(specularResult, 1.0f));
    
}