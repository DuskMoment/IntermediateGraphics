#version 450
layout(location = 0) in vec3 in_Pos;
//layout(location = 1) in vec3 in_Normal;
//layout(location = 1) in vec2 in_TexCoords;

uniform mat4 _Model;

uniform mat4 _VeiwProjection;

void main()
{

	gl_Position = _VeiwProjection * _Model * vec4(in_Pos, 1.0);
}