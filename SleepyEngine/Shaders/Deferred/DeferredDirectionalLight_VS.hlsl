#include "../Common/Transform.hlsli"
#include "../Common/VShadow.hlsli"

struct VSOut
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

VSOut main(uint vertId : SV_VertexID)
{
    VSOut output;
    
    float x = -1.0 + float((vertId & 1) << 2);
    float y = -1.0 + float((vertId & 2) << 1);
    
    output.tex.x = (x + 1.0) * 0.5;
    output.tex.y = (1.0 - y) * 0.5;
    
    output.position = float4(x, y, 0, 1);
    
    return output;
}