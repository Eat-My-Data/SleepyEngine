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

// Need to fix this one
float ShadowSL(const in float4 shadowPos, SamplerState samp)
{
    float fragDepth = shadowPos.z / shadowPos.w;
    float sampleDepth = smapSL.Sample(samp, ((float2(shadowPos.x, 1.0 - shadowPos.y) / shadowPos.w) / 2.0f) + 0.5f).r;
    float bias = 0.0005;
    return sampleDepth + bias > fragDepth;
    return smapSL.Sample(samp, shadowPos.xy).r;
    //return smapSL.SampleCmpLevelZero(ssam, shadowPos.xy, CalculateShadowDepth(shadowPos));
}

float ShadowDL(const in float4 shadowPos, SamplerState samp)
{
    float fragDepth = shadowPos.z / shadowPos.w;
    float sampleDepth = smapDL.Sample(samp, ((shadowPos.xy / shadowPos.w) / 2.0f) + 0.5f).r;
    float bias = 0.0005;
    return sampleDepth + bias > fragDepth;
    //return smapDL.SampleCmpLevelZero(ssam, shadowPos.xy, CalculateShadowDepth(shadowPos));
}

// Function found from: https://stackoverflow.com/questions/10786951/omnidirectional-shadow-mapping-with-depth-cubemap
float VectorToDepthValue(float3 Vec)
{
    // TODO: 
    // - Simplify Math for Shadow Calculation
    float3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 400.0;
    const float n = 0.5;
    float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}

float CalculatePointLightShadow(float3 viewFragPos, float3 lightPos, SamplerState splr)
{
    // get vector between fragment position and light position
    float3 fragToLight = viewFragPos - lightPos;
    
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = smapPL.Sample(splr, normalize(fragToLight)).r;

    // it is currently in linear range between [0,1]. Re-transform back to original value
    //closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = VectorToDepthValue(fragToLight);
    // now test for shadows
    float bias = 0.005;
    return closestDepth + bias > currentDepth ? 1.0 : 0.0;
}