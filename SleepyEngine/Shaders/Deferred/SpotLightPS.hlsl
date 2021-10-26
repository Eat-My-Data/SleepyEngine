#include "../Common/ShaderOps.hlsl"
#include "../Common/LightVectorData.hlsl"
#include "../Common/SpotLight.hlsl"
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
    float4 worldSpacePos = CalculateWorldPosition(float4(clipX, clipY, depthSample, 1.0));
    
    // frag position in light view and shadow map check
    float4 fragPositionInLightView = mul(worldSpacePos, spotLightData[0].spotViewProjectionMatrix);
    float shadow = CalculateSpotLightShadow(fragPositionInLightView, SampleTypePoint);

    // vector from camera to fragment
    float3 camToFrag = worldSpacePos.xyz - camPos.xyz;
        
    // attenuation
    float3 spotToFrag = spotLightData[0].pos - worldSpacePos.xyz;
    float att = saturate((1 - (length(spotToFrag) / spotLightData[0].range)));
    att *= att;
    float angularAttFactor = max(0.0f, dot(normalize(-spotLightData[0].lightDirection), normalize(spotToFrag)));
    float conAtt = saturate((angularAttFactor - spotLightData[0].outerRadius) / (spotLightData[0].innerRadius - spotLightData[0].outerRadius));

    // lighting calculations
    float3 diffuseIntensity = Diffuse(spotLightData[0].color.rgb, defaultLightIntensity, att * conAtt, normalize(-spotLightData[0].lightDirection), normalize(normals.xyz));
    float3 specularResult = Speculate(specular.xyz, 1.0f, normalize(normals.xyz), normalize(-spotLightData[0].lightDirection), camToFrag, att * conAtt, 128.0f);    

    // combined light
    float3 combinedColor = ((diffuseIntensity + specularResult) * shadow);

   	// final color
    return float4((combinedColor * colors.rgb), 1.0f);
}