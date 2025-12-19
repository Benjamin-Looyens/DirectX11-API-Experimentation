// Globals
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuse : DiffuseMap;

RasterizerState gRasterState
{
    CullMode = none;
    FrontCounterClockwise = false;
};

DepthStencilState gDepthStencState
{
    DepthEnable = true;
    DepthWriteMask = false;
    DepthFunc = less;
    StencilEnable = false;

    // Following is not needed?
    StencilReadMask = 0x0F;
    StencilWriteMask = 0x0F;

    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = always;

    FrontFaceStencilDepthFail = keep;
    BackFaceStencilDepthFail = keep;
    
    FrontFaceStencilPass = keep;
    BackFaceStencilPass = keep;
    
    FrontFaceStencilFail = keep;
    BackFaceStencilFail = keep;
};

BlendState gBlendState
{
    BlendEnable[0] = true;
    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = add;
    SrcBlendAlpha = zero;
    DestBlendAlpha = zero;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0F;
};

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
SamplerState gCurSam : CurrentSampler;

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
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuse.Sample(gCurSam, input.UV);
};

// Techniques
technique11 DefaultTechnique
{
    pass P0
    {
        SetRasterizerState(gRasterState);
        SetDepthStencilState(gDepthStencState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
};