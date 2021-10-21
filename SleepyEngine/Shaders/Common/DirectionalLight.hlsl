struct DirectionalLightData
{
    float3 lightDirection;
    float3 color;
    float att;
    float padding;
    row_major float4x4 lightViewProjectionMatrix;
};

StructuredBuffer<DirectionalLightData> directionalLightData : register(t4);

Texture2D depthTextureFromLight : register(t5);

float CalculateDirectionalLightShadow( float4 lightViewPos, SamplerState splr )
{
    float fragDepth = lightViewPos.z / lightViewPos.w;
    float sampleDepth = depthTextureFromLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
    float bias = 0.0005;
    return sampleDepth + bias > fragDepth;
}