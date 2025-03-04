#version 450

in Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
	mat3 TBN;
}fs_in;

uniform sampler2D _coords;
uniform sampler2D _Normals;
uniform sampler2D _Albito;

out vec4 FragColor;
void main()	
{

	vec3 pos = texture(_Albito, fs_in.TexCoord).rgb;

	FragColor = vec4(pos, 1.0);

}