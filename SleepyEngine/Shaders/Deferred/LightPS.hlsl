#include "../Common/ShaderOps.hlsl"

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D depthTexture : register(t3);

SamplerState SampleTypePoint : register(s0);

cbuffer LightBuffer : register(b0)
{
    float3 lightDirection;
    float padding;
    float specularIntensity;
    float att;
    float specularPower;
    float padding2;
    row_major float4x4 cameraMatrix;
    row_major float4x4 projInvMatrix;
};

cbuffer CamPosBuffer : register(b1)
{
    float3 camPos;
    float padding3;
};


float4 main(float4 position : SV_POSITION, float2 tex : TEXCOORD) : SV_TARGET
{
    // sample textures
    float4 colors = colorTexture.Load(int3(position.xy, 0));
    float4 normals = normalTexture.Sample(SampleTypePoint, tex);
    float4 specular = specularTexture.Sample(SampleTypePoint, tex);
    float depthSample = (depthTexture.Sample(SampleTypePoint, tex).r * 2.0) - 1.0;
    
    // clip space, negate y because directx
    float clipX = (tex.x * 2.0) - 1.0;
    float clipY = (tex.y * 2.0) - 1.0;
    clipY = -clipY;
    
    // normal to clip space
    normals = (normals * 2.0) - 1.0;
    
    // world position
    float4 worldDepth = float4(clipX, clipY, depthSample, 1.0);
    float4 worldPosition = mul(worldDepth, projInvMatrix);
    worldPosition /= worldPosition.w;
    float4 worldSpacePos = mul(worldPosition, cameraMatrix);
    
    // vector from camera to fragment
    float3 camToFrag = worldSpacePos.xyz - camPos.xyz;

    // diffuse light
    float diffuseIntensity = saturate(dot(normalize(normals.xyz), normalize(-lightDirection.xyz)));

    // specular
    float3 specularResult = Speculate(specular.xyz, specularIntensity, normalize(normals.xyz), normalize(-lightDirection), camToFrag, att, specularPower);

    // final color
    return colors * diffuseIntensity + float4(specularResult,1.0f);
}