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

float AttenuateSpot(const in float3 spotToFrag, const in float distFragToL)
{
    float att = saturate((1 - (distFragToL / spotLightRange)));
    att *= att;
    float angularAttFactor = max(0.0f, dot(normalize(-spotLightDirection), normalize(spotToFrag)));
    return saturate((angularAttFactor - spotLightOuterRadius) / spotLightInnerRadius - spotLightOuterRadius);
}