#include "../Common/ShaderOps.hlsl"
#include "../Common/DirectionalLight.hlsl"

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
    
    // world to camera 
    float4 worldSpacePos = CalculateWorldSpacePosition( float4(clipX, clipY, depthSample, 1.0), directionalLightData[0].projInvMatrix, directionalLightData[0].cameraMatrix );

    // world to light and shadow map check
    float4 fragPositionInLightView = mul(worldSpacePos, directionalLightData[0].lightViewProjectionMatrix);
    
    // vector from camera to fragment
    float3 camToFrag = worldSpacePos.xyz - directionalLightData[0].camPos.xyz;

    float3 ambient = {0.2f, 0.2f, 0.2f};
    
    // diffuse light
    float diffuseIntensity = max(0.0f, dot(normalize(normals.xyz), normalize(-directionalLightData[0].lightDirection.xyz)));

    float lightAtt = directionalLightData[0].att;
    float specPower = directionalLightData[0].specularPower;
    
    // specular
    float3 specularResult = Speculate(specular.xyz, directionalLightData[0].specularIntensity, normalize(normals.xyz), normalize(-directionalLightData[0].lightDirection), camToFrag, lightAtt, specPower);

    float fragDepth = fragPositionInLightView.z / fragPositionInLightView.w;
    float sampleDepth = depthTextureFromLight.Sample(SampleTypePoint, ((fragPositionInLightView.xy / fragPositionInLightView.w) / 2.0f + 0.5f)).r;
    float bias = 0.0005;
    float isInLight = sampleDepth + bias  > fragDepth;
    float3 combinedColor = ((diffuseIntensity + specularResult) * isInLight) + ambient;

   	// final color
    return float4((combinedColor * colors.rgb), 1.0f);
}