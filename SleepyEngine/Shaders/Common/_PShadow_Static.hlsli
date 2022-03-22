TextureCube smapPL : register(t3);
Texture2D smapSL : register(t4);
Texture2D smapDL : register(t5);

SamplerComparisonState ssam : register(s1);

static const float zf = 100.0f;
static const float zn = 0.5f;
static const float c1 = zf / (zf - zn);
static const float c0 = -zn * zf / (zf - zn);

float CalculateShadowDepth(const in float4 shadowPos)
{
    // get magnitudes for each basis component
    const float3 m = abs(shadowPos).xyz;
    // get the length in the dominant axis
    // (this correlates with shadow map face and derives comparison depth)
    const float major = max(m.x, max(m.y, m.z));
    // converting from distance in shadow light space to projected depth
    return (c1 * major + c0) / major;
}

float ShadowPL(const in float4 shadowPos)
{
    return smapPL.SampleCmpLevelZero(ssam, shadowPos.xyz, CalculateShadowDepth(shadowPos));
}

// TODO: These are failing
float ShadowSL(const in float4 shadowPos, SamplerState samp)
{
    //float fragDepth = shadowPos.z / shadowPos.w;
    //float sampleDepth = smapSL.Sample(samp, ((float2(shadowPos.x, 1.0 - shadowPos.y) / shadowPos.w) / 2.0f) + 0.5f).r;
    //float bias = 0.0005;
    //return sampleDepth + bias > fragDepth;
    //return smapSL.Sample(samp, shadowPos.xy).r;
    return smapSL.SampleCmpLevelZero(ssam, shadowPos.xy, CalculateShadowDepth(shadowPos));
}

float ShadowDL(const in float4 shadowPos, SamplerState samp)
{
    float fragDepth = shadowPos.z / shadowPos.w;
    float sampleDepth = smapDL.Sample(samp, ((shadowPos.xy / shadowPos.w) / 2.0f) + 0.5f).r;
    float bias = 0.0005;
    return sampleDepth + bias > fragDepth;
    //return smapDL.SampleCmpLevelZero(ssam, shadowPos.xy / shadowPos.w, CalculateShadowDepth(float4(shadowPos.xyz / shadowPos.w, shadowPos.w)));

}