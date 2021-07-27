struct PointLightStructBuf
{
    float3 viewLightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
    float3 camPos;
    float radius;
    float4x4 cameraMatrix;
    float4x4 projInvMatrix;
};

StructuredBuffer<PointLightStructBuf> pointLightData : register(t5);
