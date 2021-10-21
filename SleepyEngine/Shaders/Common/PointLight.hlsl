struct PointLightData
{
    float3 pos;
    float specularPower;
    float3 ambient;
    float diffuseIntensity;
    float3 color;
    float attConst;
    float attQuad;
    float attLin;
    float2 padding;
    float3 camPos;
    float radius;
    row_major float4x4 cameraMatrix;
    row_major float4x4 projInvMatrix;
};

StructuredBuffer<PointLightData> pointLightData : register(t6);

// TODO:
// - Make array of textures
TextureCubeArray pointLightShadowTexture[2] : register(t7);
 
cbuffer LightIndex : register(b11)
{
    float index;
    float numPointLights;
    float2 padding;
};

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
    float closestDepth = pointLightShadowTexture[lightNum].Sample(splr, normalize(float4(fragToLight,1.0f))).r;

    // it is currently in linear range between [0,1]. Re-transform back to original value
    //closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = VectorToDepthValue(fragToLight);
    // now test for shadows
    float bias = 0.0005;
    return closestDepth + bias > currentDepth ? 1.0 : 0.0;
}