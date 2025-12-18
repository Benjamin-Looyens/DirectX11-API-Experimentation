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

RasterizerState gRasterState
{
    CullMode = back; // TODO : access and change this for keybinds on C++ side
    FrontCounterClockwise = false;
};

DepthStencilState gDepthStencState
{
    DepthEnable = true;
    DepthWriteMask = true;
    DepthFunc = less;
    StencilEnable = true;
};

BlendState gBlendState
{
    BlendEnable[0] = false;
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
    //float3 World : WORLD;
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
    //output.World = mul(float4(input.Position, 1.0f), gWorld).xyz;

    return output;
};

// Shading Functions
float ObservedArea(VS_OUTPUT input, float3 normal)
{
    return saturate(dot(normal, -gLightDir));
};
float3 LambertDiffuse(VS_OUTPUT input, SamplerState sam)
{
    float3 diffuse = gDiffuse.Sample(sam, input.UV);
    
    return mul((gLightIntensity / gPI), diffuse);
};
float3 SpecularPhong(VS_OUTPUT input, SamplerState sam, float3 normal)
{
    float ks = gSpecular.Sample(sam, input.UV).x;
    float exp = gGloss.Sample(sam, input.UV).x;
    
    //float3 viewDir = normalize(input.World - gCameraPos);
    float3 lightReflect = reflect(-gLightDir, normal);
    float cosAngle = saturate(dot(normalize(lightReflect), input.ViewDir));

    return mul(ks, pow(cosAngle, mul(exp, gShininess)));
};
float3 SampleNormal(VS_OUTPUT input, SamplerState sam)
{
    float3 binormal = cross(input.Normal, input.Tangent);
    float3x3 tangentSpace = float3x3(input.Tangent, binormal, input.Normal);

    float3 normalMapSample = gNormal.Sample(sam, input.UV);
    normalMapSample = mul(2.0f, normalMapSample) - float3(1.0f, 1.0f, 1.0f);
    normalMapSample = mul(normalize(normalMapSample), tangentSpace);

    //return input.Normal;
    return normalMapSample;
}

// Pixel Shaders
float4 UniformPixelShader(VS_OUTPUT input, SamplerState sam)
{
    float3 normal = SampleNormal(input, sam);

    float observedArea = ObservedArea(input, normal);
    float3 lambert = LambertDiffuse(input, sam); 
    float3 phong = SpecularPhong(input, sam, normal);

    //return float4(mul(ObservedArea(input), LambertDiffuse(input, gSamPoint)), 1.0f); // Lambert
    //return float4(phong, 1.0f); // Specular
    return float4(((lambert + phong) * observedArea), 1.0f); // Combined
};
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
        SetRasterizerState(gRasterState);
        SetDepthStencilState(gDepthStencState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Point() ) );
    }
};
technique11 LinearTechnique
{
    pass P0
    {
        SetRasterizerState(gRasterState);
        SetDepthStencilState(gDepthStencState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Linear() ) );
    }
};
technique11 AnisotropicTechnique
{
    pass P0
    {
        SetRasterizerState(gRasterState);
        SetDepthStencilState(gDepthStencState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Anisotropic() ) );
    }
};