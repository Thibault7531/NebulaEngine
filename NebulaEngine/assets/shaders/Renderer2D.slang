struct Renderer2DUniforms
{
    float4x4 u_ViewProjection;
}

Sampler2D u_Textures[15];
ParameterBlock<Renderer2DUniforms> Renderer2DUBO : register(b1);

struct VSInput
{
    float3 a_Position;
    float4 a_Color;
    float a_TexIndex;
    float2 a_TexCoord;
    float a_TilingFactor;
    int a_EntityID;
}

struct VSOutput
{
    float4 Position : SV_Position;
    float4 v_Color;
    float v_TexIndex;
    float2 v_TexCoord;
    float v_TilingFactor;
    int v_EntityID;
}

struct FSInput
{
    float4 v_Color;
    float v_TexIndex;
    float2 v_TexCoord;
    float v_TilingFactor;
    int v_EntityID;
}

struct FSOutput
{
    float4 Color : SV_Target0;
    int EntityID : SV_Target1;
}

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    VSOutput output;

    output.Position = mul(float4(input.a_Position, 1), Renderer2DUBO.u_ViewProjection);
    output.v_Color = input.a_Color;
    output.v_TexIndex = input.a_TexIndex;
    output.v_TexCoord = input.a_TexCoord;
    output.v_TilingFactor = input.a_TilingFactor;
    output.v_EntityID = input.a_EntityID;

    return output;
}

[shader("fragment")]
FSOutput fragmentMain(FSInput input)
{
    FSOutput output;

    switch (int(input.v_TexIndex))
    {
    case 0: output.Color = u_Textures[0].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 1: output.Color = u_Textures[1].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 2: output.Color = u_Textures[2].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 3: output.Color = u_Textures[3].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 4: output.Color = u_Textures[4].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 5: output.Color = u_Textures[5].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 6: output.Color = u_Textures[6].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 7: output.Color = u_Textures[7].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 8: output.Color = u_Textures[8].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 9: output.Color = u_Textures[9].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 10: output.Color = u_Textures[10].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 11: output.Color = u_Textures[11].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 12: output.Color = u_Textures[12].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 13: output.Color = u_Textures[13].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    case 14: output.Color = u_Textures[14].Sample(input.v_TexCoord * input.v_TilingFactor); break;
    }

    output.Color *= input.v_Color;
    output.EntityID = input.v_EntityID;

    return output;
}
