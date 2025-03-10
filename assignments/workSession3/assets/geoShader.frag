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

uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _EyePos;

out vec4 FragColor;
vec3 LightDirection = vec3(0.0,-1.0, 0.0);

vec3 blinFong(vec3 WorldNormal, vec3 WorldPos)
{
	vec3 normal = normalize(WorldNormal);

	vec3 toLight = normalize(-LightDirection - WorldPos);

	float diffuseFactor = max(dot(normal,toLight),0.0);

	vec3 diffuseColor = _LightColor * diffuseFactor;

	vec3 toEye = normalize(_EyePos - WorldPos);

	vec3 h = normalize(toLight + toEye);

	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);

	vec3 lightColor = (diffuseColor * _Material.Kd + specularFactor * _Material.Ks) * _LightColor;


	return lightColor;

}
void main()	
{

	vec3 pos = texture(_Albito, fs_in.TexCoord).rgb;
	vec3 color = blinFong(texture(_Normals, fs_in.TexCoord).xyz, texture(_coords, fs_in.TexCoord).xyz);

	color *= pos;

	FragColor = vec4(color, 1.0);

}
