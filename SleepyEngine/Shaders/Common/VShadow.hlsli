cbuffer PLShadowTransformCBuf : register(b1)
{
    matrix shadowPosPL;
};

cbuffer SLShadowTransformCBuf : register(b2)
{
    row_major matrix shadowPosSL;
};

cbuffer DLShadowTransformCBuf : register(b3)
{
    row_major matrix shadowPosDL;
};

float4 ToShadowHomoSpace(const in float3 pos, uniform matrix modelTransform, matrix shadowPos)
{
    const float4 world = mul(float4(pos, 1.0f), modelTransform);
    return mul(world, shadowPos);
}