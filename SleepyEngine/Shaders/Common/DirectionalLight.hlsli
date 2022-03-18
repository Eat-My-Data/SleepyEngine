cbuffer DirectionalLightCBuf : register(b5)
{
    float3 dirLightDirection;
    float3 dirLightColor;
    float3 dirLightAmbient;
    float dirLightIntensity;
    float2 dirLightPadding;
    row_major float4x4 dirLightViewProjectionMatrix;
};

Texture2D depthTextureFromLight : register(t5);

//float CalculateDirectionalLightShadow( float4 lightViewPos, SamplerState splr )
//{
//    float fragDepth = lightViewPos.z / lightViewPos.w;
//    float sampleDepth = depthTextureFromLight.Sample(splr, ((lightViewPos.xy / lightViewPos.w) / 2.0f) + 0.5f).r;
//    float bias = 0.0005;
//    return sampleDepth + bias > fragDepth;
//}