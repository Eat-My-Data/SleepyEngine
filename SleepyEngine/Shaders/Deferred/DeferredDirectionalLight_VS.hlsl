#include "../Common/Transform.hlsli"
#include "../Common/VShadow.hlsli"

struct VSOut
{
    float4 position : SV_POSITION;
};

VSOut main(float3 pos : Position)
{
    VSOut output;
    output.position = mul(float4(pos, 1.0f), modelViewProj);
    return output;
}