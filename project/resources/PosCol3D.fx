// Globals
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuse : DiffuseMap;

SamplerState gSamPoint
{
    Filter = MIN_MAG_MIP_POINT; // Point | Linear | Anisotropic
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

// Pixel Shader
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuse.Sample(gSamPoint, input.UV);
    //return float4(gDiffuse.Sample(gSamPoint, input.UV), 1.0f);
    //return float4(input.Color, 1.0f);
};

// Technique
technique11 DefaultTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
};