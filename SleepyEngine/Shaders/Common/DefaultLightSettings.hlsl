cbuffer DefaultLightSettings : register(b7)
{
    float3 defaultAmbientLight;
    float defaultSpecularPower;
    float defaultLightIntensity;
    float3 padding;
};