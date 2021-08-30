struct DirectionalLightData
{
    float4 color;
    float3 lightDirection;
    float padding0;
    float specularIntensity;
    float att;
    float specularPower;
    float padding1;
    float3 camPos;
    float padding2;
    row_major float4x4 cameraMatrix;
    row_major float4x4 projInvMatrix;
    row_major float4x4 lightViewProjectionMatrix;
};

StructuredBuffer<DirectionalLightData> directionalLightData : register(t5);

// TODO:
// - Make more re usable code 