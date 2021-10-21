cbuffer CameraData : register(b6)
{
    float4 camPos;
    row_major float4x4 viewInvMatrix;
    row_major float4x4 projInvMatrix;
};