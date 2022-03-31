cbuffer CameraData : register(b10)
{
     float4x4 viewInvMatrix;
     float4x4 projInvMatrix;
    float4 camPos;
};

float4 CalculateWorldPosition(float4 worldDepth)
{
    float4 worldPosition = mul(worldDepth, projInvMatrix);
    worldPosition /= worldPosition.w;
    return mul(worldPosition, viewInvMatrix);
}