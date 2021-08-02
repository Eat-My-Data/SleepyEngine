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