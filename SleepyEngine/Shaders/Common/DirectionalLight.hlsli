cbuffer DirectionalLightCBuf : register(b5)
{
    float3 dirLightDirection;
    float3 dirLightColor;
    float3 dirLightAmbient;
    float dirLightIntensity;
    float3 dirLightPadding;
    row_major float4x4 dirLightViewProjectionMatrix;
};

Texture2D depthTextureFromLight : register(t5);