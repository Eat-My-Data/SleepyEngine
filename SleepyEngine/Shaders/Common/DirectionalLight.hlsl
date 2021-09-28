struct DirectionalLightData
{
    float4 color;
    float3 lightDirection;
    float padding0;
    float specularIntensity;
    float att;
    float specularPower;
    float padding1;
    float3 camPos;
    float padding2;
    row_major float4x4 cameraMatrix;
    row_major float4x4 projInvMatrix;
    row_major float4x4 lightViewProjectionMatrix;
};

StructuredBuffer<DirectionalLightData> directionalLightData : register(t5);

Texture2D depthTextureFromLight : register(t4);

float CalculateDirectionalLightShadow( float4 lightViewPos, SamplerState splr )
{
    float fragDepth = lightViewPos.z / lightViewPos.w;
    float sampleDepth = depthTextureFromLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
    float bias = 0.0005;
    return sampleDepth + bias > fragDepth;
}