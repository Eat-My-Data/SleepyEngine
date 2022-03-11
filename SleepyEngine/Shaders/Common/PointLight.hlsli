cbuffer PointLightCBuf : register(b0)
{
    float3 viewLightPos;
    float3 ambient;
    float3 diffuseColor;
    float3 radius;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};