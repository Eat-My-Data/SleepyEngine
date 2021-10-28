#include "./PointLight.hlsl"

cbuffer SolidGeometryColor : register(b8)
{
    float3 color;
}    

float4 main() : SV_TARGET
{
    return float4(color, 1.0f);
}