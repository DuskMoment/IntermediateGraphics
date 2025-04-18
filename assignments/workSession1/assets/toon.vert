#version 450

//vertex attributes
layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoords;
//layout(location = 3) in vec3 in_Tangent;

uniform mat4 _Model;

uniform mat4 _VeiwProjection;


out Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
	//mat3 TBN;
}vs_out;


void main()
{
	//model to world space converstions
	vs_out.WorldPos = vec3(_Model * vec4(in_Pos,1.0));
	vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * in_Normal;

	//vec3 T = normalize(vec3(_Model * vec4(in_Tangent, 0.0)));
	//vec3 N = normalize(vec3(_Model * vec4(in_Normal, 0.0)));

	//re-orthogonalize T with respect to N 
	//T = normalize(T - dot(T,N) * N);

	//vec3 B = normalize(vec3(_Model * vec4(in_biTangnet, 0.0)));
	//vec3 B = cross(N,T);

	//vs_out.TBN = transpose(mat3(T,B,N));
	//vs_out.TBN = mat3(T,B,N);

	//texutre
	vs_out.TexCoord = in_TexCoords;

	gl_Position = _VeiwProjection * _Model * vec4(in_Pos, 1.0);
}