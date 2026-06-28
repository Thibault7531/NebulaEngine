#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in float a_TexIndex;
layout(location = 3) in vec2 a_TexCoord;
layout(location = 4) in int a_EntityID;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out float v_TexIndex;
out vec2 v_TexCoord;
flat out int v_EntityID;

void main()
{
    v_Color = a_Color;
    v_TexIndex = a_TexIndex;
	v_TexCoord = a_TexCoord;
	v_EntityID = a_EntityID;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out int entity;

in vec4 v_Color;
in float v_TexIndex;
in vec2 v_TexCoord;
flat in int v_EntityID;

uniform float u_TilingFactor;
uniform sampler2D u_Textures[16];

void main()
{
	switch(int(v_TexIndex))
	{
		case 0: color = texture(u_Textures[0], v_TexCoord); break;
		case 1: color = texture(u_Textures[1], v_TexCoord); break;
		case 2: color = texture(u_Textures[2], v_TexCoord); break;
		case 3: color = texture(u_Textures[3], v_TexCoord); break;
		case 4: color = texture(u_Textures[4], v_TexCoord); break;
		case 5: color = texture(u_Textures[5], v_TexCoord); break;
		case 6: color = texture(u_Textures[6], v_TexCoord); break;
		case 7: color = texture(u_Textures[7], v_TexCoord); break;
		case 8: color = texture(u_Textures[8], v_TexCoord); break;
		case 9: color = texture(u_Textures[9], v_TexCoord); break;
		case 10: color = texture(u_Textures[10], v_TexCoord); break;
		case 11: color = texture(u_Textures[11], v_TexCoord); break;
		case 12: color = texture(u_Textures[12], v_TexCoord); break;
		case 13: color = texture(u_Textures[13], v_TexCoord); break;
		case 14: color = texture(u_Textures[14], v_TexCoord); break;
		case 15: color = texture(u_Textures[15], v_TexCoord); break;
	}

	color *= v_Color;
	entity = v_EntityID;
}