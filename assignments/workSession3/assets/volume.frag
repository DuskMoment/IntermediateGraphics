#version 450
out vec4 FragColor;

in Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
	mat3 TBN;
}fs_in;

const int MAX_SUZAN = 100;
struct Light
{
	vec3 pos;
	vec3 color;
	float radius;
};

uniform Light _lights;

uniform sampler2D _albito;
uniform sampler2D _normals; 
uniform sampler2D _positions;

uniform vec3 _EyePos;

struct Material{
	float Ka;
	float Kd;
	float Ks;
	float Shininess;
	
};
uniform Material _Material;

float attenuateLinear(float dist, float radius){
	return clamp((radius-dist)/radius,0.0,1.0);
}

vec3 calcPointLight(Light light, vec3 normal, vec3 pos)
{

	vec3 diff = light.pos - pos;
	//Direction toward light position
	vec3 toLight = normalize(diff);
	//TODO: Usual blinn-phong calculations for diffuse + specular
	float diffuseFactor = max(dot(normal,toLight),0.0);

	vec3 diffuseColor = light.color * diffuseFactor;

	vec3 toEye = normalize(_EyePos - pos);

	vec3 h = normalize(toLight + toEye);

	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);

	vec3 lightColor = (diffuseColor * _Material.Kd + specularFactor * _Material.Ks) * light.color;

	//Attenuation
	float d = length(diff); //Distance to light
	//lightColor *=attenuateLinear(d,light.radius); //See below for attenuation options
	return lightColor;
}

//Linear falloff

void main()
{
	vec2 UV = gl_FragCoord.xy / textureSize(_normals,0);

	vec3 normal = texture(_normals, UV).rgb;
	vec3 worldPos = texture(_positions, UV).rgb;

	Light light = _lights;
	vec3 lightColor = calcPointLight(light, normal, worldPos);

	FragColor = vec4(lightColor * texture(_albito, UV).rgb, 1.0);

}