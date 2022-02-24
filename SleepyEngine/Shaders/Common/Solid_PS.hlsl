#include "./PointLight.hlsli"

cbuffer SolidGeometryColor : register(b8)
{
    float3 materialColor;
}    

float4 main() : SV_TARGET
{
    return float4(materialColor, 1.0f);
}