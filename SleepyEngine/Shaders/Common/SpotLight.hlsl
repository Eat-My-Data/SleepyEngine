struct SpotLightData
{
    float4 color;
    float3 lightDirection;
    float range;
    float3 pos;
    float cone;
};

StructuredBuffer<SpotLightData> spotLightData : register(t10);

Texture2D depthTextureFromLight : register(t11);

float CalculateSpotLightShadow( float4 lightViewPos, SamplerState splr )
{
    float fragDepth = lightViewPos.z / lightViewPos.w;
    float sampleDepth = depthTextureFromLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
    return sampleDepth > fragDepth;
}