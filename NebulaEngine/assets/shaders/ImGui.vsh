#version 410 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_UV;
layout (location = 2) in vec4 a_Color;

uniform mat4 ProjMtx;

out vec2 v_UV;
out vec4 v_Color;

void main() {
    v_UV = a_UV;
    v_Color = a_Color;
    gl_Position = ProjMtx * vec4(a_Position.xy, 0, 1);
}
