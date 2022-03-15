#include "../Common/ShaderOps.hlsli"
#include "../Common/LightVectorData.hlsli"
#include "../Common/PointLight.hlsli"
#include "../Common/Camera.hlsli"
#include "../Common/PShadow.hlsli"
#include "../Common/DirectionalLight.hlsli"

Texture2D colorTexture : register(t4);
Texture2D normalTexture : register(t5);
Texture2D specularTexture : register(t6);
Texture2D depthTexture : register(t7);

TextureCube shadowMap : register(t3); // shadow map

SamplerState SampleTypePoint : register(s0);


float4 main(float4 position : SV_POSITION, float2 tex : TEXCOORD) : SV_TARGET
{
    // sample textures
    float4 colors = colorTexture.Load(int3(position.xy, 0));
    float4 normals = normalTexture.Sample(SampleTypePoint, tex);
    float4 specular = specularTexture.Sample(SampleTypePoint, tex);
    float depthSample = depthTexture.Sample(SampleTypePoint, tex).r;
    
    // clip space, negate y because directx
    float clipX = (tex.x * 2.0) - 1.0;
    float clipY = (tex.y * 2.0) - 1.0;
    clipY = -clipY;
    
    // normal to clip space
    normals = (normals * 2.0) - 1.0;
    
    // world position 
    float4 worldSpacePos = CalculateWorldPosition(float4(clipX, clipY, depthSample, 1.0));

    // frag position in light view and shadow map check
    float4 fragPositionInLightView = mul(worldSpacePos, lightViewProjectionMatrix);
    float shadow = CalculateDirectionalLightShadow(fragPositionInLightView, SampleTypePoint);

    // vector from camera to fragment
    float3 camToFrag = worldSpacePos.xyz - camPos.xyz;
    
    // attenuation    
    // lighting calculations
    float3 diffuseIntensity = Diffuse(color, 1.0f, 1.0f, normalize(-lightDirection), normalize(normals.xyz));
    float3 specularResult = Speculate(specular.xyz, 1.0f, normalize(normals.xyz), normalize(-lightDirection), camToFrag, 1.0f, 128);

    // combined light
    float3 combinedColor = ((diffuseIntensity + specularResult) * shadow) + dirAmbient;

   	// final color
    return float4((combinedColor * colors.rgb), 1.0f);
}