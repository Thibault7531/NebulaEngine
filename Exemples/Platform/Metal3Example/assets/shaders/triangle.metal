#include <metal_stdlib>
using namespace metal;

struct VSInput {
    packed_float3 position;
    packed_float3 color;
};

struct VSOutput {
    float4 position [[position]];
    float3 color;
};

vertex VSOutput vertexShader(uint vertexID [[vertex_id]], constant VSInput* vertices) {
    VSOutput output;
    
    output.position = float4(vertices[vertexID].position[0],
                             vertices[vertexID].position[1],
                             vertices[vertexID].position[2],
                             1.0f);
    output.color = vertices[vertexID].color;
    
    return output;
}

fragment float4 fragmentShader(VSOutput in [[stage_in]]) {
    return float4(in.color, 1.0f);
}
