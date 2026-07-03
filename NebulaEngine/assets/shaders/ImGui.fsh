#version 410 core

in vec2 v_UV;
in vec4 v_Color;

uniform sampler2D Texture;

layout (location = 0) out vec4 Out_Color;

void main() {
    Out_Color = v_Color * texture(Texture, v_UV.st);
}