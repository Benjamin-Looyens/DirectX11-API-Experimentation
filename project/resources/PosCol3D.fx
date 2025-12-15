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
    Filter = ANISOTROPIC; // Is this correct?
    AddressU = Wrap;
    AddressV = Wrap;
};

// Input
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 UV : TEXCOORD;
};

// Output
struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
    float2 UV : TEXCOORD;
};

// Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Position = float4(input.Position, 1.0f);
    output.Position = mul(output.Position, gWorldViewProj);

    output.Color = input.Color;
    output.UV = input.UV;
    return output;
};

// Pixel Shaders
float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuse.Sample(gSamPoint, input.UV);
};
float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuse.Sample(gSamLinear, input.UV);
};
float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuse.Sample(gSamAnisotropic, input.UV);
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