#include "../Common/Transform.hlsl"

struct VSOut
{
    float4 position : SV_POSITION;
};

VSOut main(float3 viewFragPos : Position)
{
    VSOut output;
    
    output.position = mul(float4(viewFragPos, 1.0f), modelViewProj);
 
    return output;
}