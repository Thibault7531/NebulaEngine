#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_Position;
out vec3 v_Normal;

void main()
{
    v_Normal = a_Normal;
    v_Position = a_Position;
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout (location = 0) out vec4 color;

in vec3 v_Position;
in vec3 v_Normal;

void main()
{
    color = vec4(vec3(1.0, 1.0, 1.0) * dot(normalize(v_Normal), normalize(vec3(0.0, 0.0, 0.0) - v_Position)), 1.0);
}