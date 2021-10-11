struct SpotLightData
{
    float4 color;
    float3 lightDirection;
    float range;
    float3 pos;
    float cone;
    float4x4 spotViewProjectionMatrix;
};

StructuredBuffer<SpotLightData> spotLightData : register(t10);

Texture2D depthTextureFromSpotLight : register(t11);

float CalculateSpotLightShadow( float4 lightViewPos, SamplerState splr)
{
    // inner and outer radius angle, the inner has full light intensity and the outer stops getting light
    float fragDepth = lightViewPos.z / lightViewPos.w;
    float sampleDepth = depthTextureFromSpotLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
    float bias = 0.0005;
    return sampleDepth + bias > fragDepth;
}