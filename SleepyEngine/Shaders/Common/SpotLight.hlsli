cbuffer SpotLightCBuf : register(b4)
{
    float3 spotLightPos;
    float3 spotLightColor;
    float3 spotLightDirection;
    float spotLightRange;
    float spotLightOuterRadius;
    float spotLightInnerRadius;
    float spotLightPitch;
    float spotLightYaw;
    float2 padding;
    row_major float4x4 spotViewProjectionMatrix;
};

TextureCube spotLightShadowMap : register(t4);
