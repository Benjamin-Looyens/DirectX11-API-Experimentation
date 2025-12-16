// Globals
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuse : DiffuseMap;

SamplerState gSamPoint
{
    Filter = MIN_MAG_MIP_POINT; 
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState gSamLinear
{
    Filter = MIN_MAG_MIP_LINEAR; 
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState gSamAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

// Input
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
};

// Output
struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

// Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Position = float4(input.Position, 1.0f);
    output.Position = mul(output.Position, gWorldViewProj);

    output.UV = input.UV;

    return output;
};

// Pixel Shaders
float4 UniformPixelShader(VS_OUTPUT input, SamplerState sam)
{

    float3 diffuse = gDiffuse.Sample(sam, input.UV);
    return float4(diffuse, 1.0f);

    //return gDiffuse.Sample(sam, input.UV); // TODO : Implement alpha channel
}

float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    return UniformPixelShader(input, gSamPoint);
};
float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
    return UniformPixelShader(input, gSamLinear);
};
float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
    return UniformPixelShader(input, gSamAnisotropic);
};

// Techniques
technique11 PointTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Point() ) );
    }
};
technique11 LinearTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Linear() ) );
    }
};
technique11 AnisotropicTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Anisotropic() ) );
    }
};