cbuffer ModelMatrix : register(b0)
{
    matrix modelMatrix;
};

cbuffer ViewProjectionMatrix : register(b1)
{
    row_major matrix viewProjectionMatrix;
};

float4 main( float3 pos : POSITION ) : SV_POSITION
{
    float4 newPos = mul(float4(pos, 1.0), modelMatrix);
    //newPos = mul(newPos, viewProjectionMatrix);
    return newPos;
}