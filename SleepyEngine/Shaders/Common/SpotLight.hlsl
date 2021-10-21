struct SpotLightData
{
    float4 color;
    float3 lightDirection;
    float range;
    float3 pos;
    float outerRadius;
    float innerRadius;
    float3 padding;
    float4x4 spotViewProjectionMatrix;
};

StructuredBuffer<SpotLightData> spotLightData : register(t8);

Texture2D depthTextureFromSpotLight : register(t9);

float CalculateSpotLightShadow(float4 lightViewPos, SamplerState splr)
{
    float fragDepth = lightViewPos.z / lightViewPos.w;
    float sampleDepth = depthTextureFromSpotLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
    float bias = 0.0005;
    return sampleDepth + bias > fragDepth;
}