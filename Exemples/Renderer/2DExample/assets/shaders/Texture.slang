struct RendererUniform
{
    float4x4 u_ViewProjection;
    float4x4 u_Transform;
}

Sampler2D u_Texture;
ParameterBlock<RendererUniform> RendererUBO : register(b1);

struct VSInput
{
    float3 a_Position;
    float2 a_TexCoord;
}

struct VSOutput
{
    float4 position : SV_Position;
    float2 v_TexCoord;
}

struct FSInput
{
    float2 v_TexCoord;
}

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    VSOutput output;

    output.position = mul(mul(float4(input.a_Position, 1.0), RendererUBO.u_Transform), RendererUBO.u_ViewProjection);
    output.v_TexCoord = input.a_TexCoord;

    return output;
}

[shader("fragment")]
float4 fragmentMain(FSInput input) : SV_Target
{
    return u_Texture.Sample(input.v_TexCoord);
}
