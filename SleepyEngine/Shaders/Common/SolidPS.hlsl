#include "./PointLight.hlsl"

float4 main() : SV_TARGET
{
    return float4(pointLightData[0].color, 1.0f);
}