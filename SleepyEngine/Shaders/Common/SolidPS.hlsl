#include "./PointLight.hlsl"

float4 main() : SV_TARGET
{
    return float4(pointLightData[index].color, 1.0f);
}