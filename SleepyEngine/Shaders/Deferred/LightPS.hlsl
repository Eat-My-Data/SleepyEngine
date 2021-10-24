#include "../Common/ShaderOps.hlsl"
#include "../Common/DirectionalLight.hlsl"
#include "../Common/CameraData.hlsl"
#include "../Common/DefaultLightSettings.hlsl"

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D depthTexture : register(t3);

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
    float4 fragPositionInLightView = mul(worldSpacePos, directionalLightData[0].lightViewProjectionMatrix);
    float shadow = CalculateDirectionalLightShadow(fragPositionInLightView, SampleTypePoint);

    // vector from camera to fragment
    float3 camToFrag = worldSpacePos.xyz - camPos.xyz;
    
    // attenuation
    float lightAtt = directionalLightData[0].att;
    
    // lighting calculations
    float3 diffuseIntensity = Diffuse(directionalLightData[0].color, defaultLightIntensity, lightAtt, normalize(-directionalLightData[0].lightDirection), normalize(normals.xyz));
    float3 specularResult = Speculate(specular.xyz, defaultLightIntensity, normalize(normals.xyz), normalize(-directionalLightData[0].lightDirection), camToFrag, lightAtt, defaultSpecularPower);

    // combined light
    float3 combinedColor = ((diffuseIntensity + specularResult) * shadow) + defaultAmbientLight;

   	// final color
    return float4((combinedColor * colors.rgb), 1.0f);
}