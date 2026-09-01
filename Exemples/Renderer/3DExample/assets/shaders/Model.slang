struct RendererUniforms
{
    float4x4 u_ViewProjection;
    float4x4 u_Transform;
}

ConstantBuffer<RendererUniforms> RendererUBO;

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

    output.v_Position = mul(float4(input.a_Position, 1), RendererUBO.u_Transform).xyz;
    output.Position = mul(float4(output.v_Position, 1.0), RendererUBO.u_ViewProjection);
    output.v_Normal = input.a_Normal;

    return output;
}

[shader("fragment")]
float4 fragmentMain(FSInput input)
{
    float3 lightPosition = float3(0, 0, -10);
    float3 lightDirection = normalize(input.v_Position - lightPosition);
    float3 normalizedNormal = normalize(input.v_Normal);
    float3 luminance = abs(min(dot(lightDirection, normalizedNormal), 0));

    return float4(luminance * float3(1, 1, 1), 1);
}
