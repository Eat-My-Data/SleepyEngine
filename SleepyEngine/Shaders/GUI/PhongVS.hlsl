cbuffer TransformCBuf
{
    matrix GUIProjection;
}; 

struct VSOut
{
    float4 pos : SV_POSITION;
};

VSOut main(float2 pos : Position)
{
    VSOut vso;
    vso.pos = mul(float4(pos, 0.0f, 1.0f), GUIProjection);
    return vso;
}
