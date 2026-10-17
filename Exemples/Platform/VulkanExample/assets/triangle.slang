struct VSInput {
    float3 a_Position;
    float3 a_Color;
}

struct VSOutput {
    float4 position : SV_Position;
    float3 v_Color;
}

struct FSInput {
    float3 v_Color;
}

[shader("vertex")]
VSOutput vertexMain(VSInput input) {
    VSOutput output;
    output.position = float4(input.a_Position, 1.0);
    output.v_Color = input.a_Color;
    return output;
}

[shader("fragment")]
float4 fragmentMain(FSInput input) {
    return float4(input.v_Color, 1.0);
}