struct PointLightData
{
    float3 color;
    float3 pos;
    float radius;
    float padding;
};

StructuredBuffer<PointLightData> pointLightData : register(t6);

cbuffer LightIndex : register(b9)
{
    int index;
    int numPointLights;
    float2 index_padding;
}

// TODO:
// - Make array of textures
TextureCube pointLightShadowTexture : register(t7);

// Function found from: https://stackoverflow.com/questions/10786951/omnidirectional-shadow-mapping-with-depth-cubemap
float VectorToDepthValue(float3 Vec)
{
    // TODO: 
    // - Simplify Math for Shadow Calculation
    float3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 200.0;
    const float n = 0.001;
    float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}

float CalculatePointLightShadow(float3 viewFragPos, float3 lightPos, SamplerState splr, float lightNum )
{
    // get vector between fragment position and light position
    float3 fragToLight = viewFragPos - lightPos;
    
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = pointLightShadowTexture.Sample(splr, normalize(fragToLight)).r;

    // it is currently in linear range between [0,1]. Re-transform back to original value
    //closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = VectorToDepthValue(fragToLight);
    // now test for shadows
    float bias = 0.0005;
    return closestDepth + bias > currentDepth ? 1.0 : 0.0;
}