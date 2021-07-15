cbuffer Color
{
    float4 color;
};

float4 main(float4 pos : SV_Position) : SV_TARGET
{ 
    return color;
}