struct RendererUniforms
{
    float4x4 u_ViewProjection;
    float4x4 u_Transform;
}

ParameterBlock<RendererUniforms> RendererUBO;

struct VSInput
{
    float3 a_Position;
    float3 a_Normal;
}

struct VSOutput
{
    float4 Position : SV_Position;
    float3 v_Position;
    float3 v_Normal;
}

struct FSInput
{
    float3 v_Position;
    float3 v_Normal;
}

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    VSOutput output;

    output.Position = mul(mul(float4(input.a_Position, 1.0), RendererUBO.u_Transform), RendererUBO.u_ViewProjection);
    output.v_Position = input.a_Position;
    output.v_Normal = input.a_Normal;

    return output;
}

[shader("fragment")]
float4 fragmentMain(FSInput input)
{
    return float4(dot(normalize(input.v_Normal), normalize(float3(0, 0, 0) - input.v_Position)) * float3(1, 1, 1), 1.0);
}
