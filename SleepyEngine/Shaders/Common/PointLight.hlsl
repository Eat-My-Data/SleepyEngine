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
TextureCube pointLightShadowTexture2 : register(t8);

cbuffer LightIndex : register(b10)
{
    float index;
    float numPointLights;
    float2 padding;
};

float VectorToDepthValue(float3 Vec)
{
    float3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 2048.0;
    const float n = 1.0;
    float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}

float CalculatePointLightShadow(float3 viewFragPos, float3 lightPos, SamplerState splr, float lightNum )
{
    // get vector between fragment position and light position
    float3 fragToLight = viewFragPos - lightPos;
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = 1.0f;
    if ( lightNum == 0 ) 
        closestDepth = pointLightShadowTexture.Sample(splr, normalize(fragToLight)).r;
    else if ( lightNum == 1 )
        closestDepth = pointLightShadowTexture2.Sample(splr, normalize(fragToLight)).r;

    // it is currently in linear range between [0,1]. Re-transform back to original value
    //closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = VectorToDepthValue(fragToLight);
    // now test for shadows
    float bias = 0.05;
    return closestDepth + bias > currentDepth ? 1.0 : 0.0;
}