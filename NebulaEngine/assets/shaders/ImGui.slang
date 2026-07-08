struct ImGuiRendererUniforms
{
    float4x4 u_ProjectionMatrix;
}

Sampler2D u_Texture;
ParameterBlock<ImGuiRendererUniforms> ImGuiUBO : register(b1);

struct VSInput
{
    float2 a_Position;
    float2 a_UV;
    float4 a_Color;
}

struct VSOuput
{
    float4 Position : SV_Position;
    float2 v_UV;
    float4 v_Color;
}

struct FSInput
{
    float2 v_UV;
    float4 v_Color;
}

[shader("vertex")]
VSOuput vertexMain(VSInput input)
{
    VSOuput output;

    output.Position = mul(float4(input.a_Position, 0, 1), ImGuiUBO.u_ProjectionMatrix);
    output.v_UV = input.a_UV;
    output.v_Color = input.a_Color;

    return output;
}

[shader("fragment")]
float4 fragmentMain(FSInput input)
{
    return input.v_Color * u_Texture.Sample(input.v_UV);
}
