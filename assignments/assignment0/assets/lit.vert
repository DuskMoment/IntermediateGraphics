#version 450

//vertex attributes
layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoords;
uniform mat4 _Model;
uniform mat4 _VeiwProjection;

out vec3 out_Normal;

out Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
}vs_out;


void main()
{
	//transform vertex pos intro world space

	vs_out.WorldPos = vec3(_Model * vec4(in_Pos,1.0));
	vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * in_Normal;
	


	out_Normal = in_Normal;

	//vs_out.Normal = in_Normal;
	vs_out.TexCoord = in_TexCoords;

	gl_Position = _VeiwProjection * _Model * vec4(in_Pos, 1.0);
}