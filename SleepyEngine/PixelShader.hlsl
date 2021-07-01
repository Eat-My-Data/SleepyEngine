float4 main(float4 pos : SV_POSITION, float3 normal : NORMAL) : SV_TARGET
{ 
    float4 output;
    
    float3 lightDirection = { 0.0f, 1.0f, 0.0f };
    output = float4(dot(lightDirection.xyz, normal), 0.0f, 0.0f, 1.0f);
    
    return output;
}