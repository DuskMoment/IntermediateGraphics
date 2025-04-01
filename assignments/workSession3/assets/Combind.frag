#version 450

in Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
	mat3 TBN;
}fs_in;

struct Material{
	float Ka;
	float Kd;
	float Ks;
	float Shininess;
	
};

//uniforms
uniform Material _Material;

uniform sampler2D _coords;
uniform sampler2D _Normals;
uniform sampler2D _Albito;

uniform sampler2D _Volume;

vec3 _LightColor = vec3(1.0);
uniform vec3 _EyePos;

out vec4 FragColor;
vec3 LightDirection = vec3(0.0,-1.0, 0.0);

const int MAX_SUZAN = 100;
struct Light
{
	vec3 pos;
	vec3 color;
	float radius;
};
uniform Light _lights[MAX_SUZAN];

void main()	
{

	vec3 texColor = texture(_Albito, fs_in.TexCoord).rgb;
	vec3 volume = texture(_Volume, fs_in.TexCoord).rgb;

	vec3 color; //= blinFong(texture(_Normals, fs_in.TexCoord).xyz, texture(_coords, fs_in.TexCoord).xyz, _LightColor, LightDirection);

    color = texColor * volume;

	FragColor = vec4(color, 1.0);

}
