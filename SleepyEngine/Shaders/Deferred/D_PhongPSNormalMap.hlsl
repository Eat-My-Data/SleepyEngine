Texture2D tex;

SamplerState splr;

struct PSOut
{
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
    float4 specular : SV_Target2;
};


PSOut main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord)
{
    PSOut output;
    
    // Sample the color from the texture and store it for output to the render target.
    output.color = tex.Sample(splr, tc);
	
    // 0 to 1 for normal
    viewNormal = normalize(viewNormal);
    viewNormal = (viewNormal + 1) / 2;
    
    // Store the normal for output to the render target.
    output.normal = float4(viewNormal, 1.0f);
    output.specular = float4(0, 0, 0, 0);
    
    return output;
}