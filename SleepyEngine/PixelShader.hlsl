float4 main(float3 viewFragPos : SV_Position, float3 viewNormal : Normal) : SV_TARGET
{ 
    float4 output;
    
    float3 lightDirection = { 0.0f, 1.0f, 0.0f };
    output = float4(dot(lightDirection.xyz, viewNormal), 0.0f, 0.0f, 1.0f);
    
    return output;
}