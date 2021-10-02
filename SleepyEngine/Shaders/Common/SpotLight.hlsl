struct SpotLightData
{
    float4 color;
    float3 lightDirection;
    float range;
    float3 pos;
    float cone;
};

StructuredBuffer<SpotLightData> spotLightData : register(t10);

Texture2D depthTextureFromSpotLight : register(t11);

float VectorToDepthValue2(float3 Vec)
{
    float3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 200.0;
    const float n = 0.001;
    float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}

float CalculateSpotLightShadow(float3 viewFragPos, float4 lightViewPos, SamplerState splr)
{
    float3 fragToLight = viewFragPos - lightViewPos.xyz;
    float sampleDepth = depthTextureFromSpotLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
    float bias = 0.0005;
    float currentDepth = VectorToDepthValue2(fragToLight);
    return sampleDepth + bias > currentDepth;
}