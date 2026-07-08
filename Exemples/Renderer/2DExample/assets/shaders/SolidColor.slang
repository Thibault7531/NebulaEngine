struct RendererUniform
{
    float4x4 u_ViewProjection;
    float4x4 u_Transform;
}

struct ColorUniform
{
    float3 u_Color;
}

ParameterBlock<RendererUniform> RendererUBO : register(b0);
ParameterBlock<ColorUniform> ColorUBO : register(b1);

struct VSInput
{
    float3 a_Position;
}

struct VSOutput
{
    float4 position : SV_Position;
    float3 v_Position;
}

struct FSInput
{
    float3 v_Position;
}

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    VSOutput output;

    output.position = mul(mul(float4(input.a_Position, 1.0), RendererUBO.u_Transform), RendererUBO.u_ViewProjection);
    output.v_Position = input.a_Position;

    return output;
}

[shader("fragment")]
float4 fragmentMain(FSInput input) : SV_Target
{
    return float4(ColorUBO.u_Color, 1.0);
}