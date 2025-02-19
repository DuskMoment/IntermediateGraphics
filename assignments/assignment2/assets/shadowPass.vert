#version 450

//vertex attributes
layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoords;
layout(location = 3) in vec3 in_Tangent;

uniform mat4 _Model;

uniform mat4 _VeiwProjection;


out Surface{
	vec4 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
	mat3 TBN;
}vs_out;


void main()
{
	vs_out.WorldPos = _Model * vec4(in_Pos, 1.0);
	gl_Position = _VeiwProjection * vs_out.WorldPos;
}
