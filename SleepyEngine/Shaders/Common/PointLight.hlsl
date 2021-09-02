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

TextureCube pointLightShadowTexture : register(t7);

float CalculatePointLightShadow(float3 viewFragPos, float3 lightPos, SamplerState splr, float farPlane )
{
    // get vector between fragment position and light position
    float3 fragToLight = viewFragPos - lightPos;
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = pointLightShadowTexture.Sample(splr, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float bias = 0.05;
    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}