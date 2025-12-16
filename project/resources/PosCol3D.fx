// Globals
float gPI = 3.14159265358979323846264338327950288f; // Kinda yuck no?
float gShininess = 25.0f;
float gLightIntensity = 7.0f;
float3 gLightDir = float3(0.577f, -0.577f, 0.576f);
float3 gCameraPos : CameraPosition;
float4x4 gWorld : World;
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuse : DiffuseMap;
Texture2D gNormal : NormalMap;
Texture2D gSpecular : SpecularMap;
Texture2D gGloss : GlossinessMap;

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
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

// Output
struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 ViewDir : VIEW_DIRECTION; 
};

// Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Position = float4(input.Position, 1.0f);
    output.Position = mul(output.Position, gWorldViewProj);

    output.Color = input.Color;
    
    output.UV = input.UV;
    
    output.Normal = normalize(mul(input.Normal, (float3x3)gWorld));
    output.Tangent = normalize(mul(input.Tangent, (float3x3)gWorld));

    output.ViewDir = normalize(mul(float4(input.Position, 1.0f), gWorld).xyz - gCameraPos); // Calculate view direction for current vertex, interpolate later

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