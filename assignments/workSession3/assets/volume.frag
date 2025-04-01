#version 450
layout(location = 0) out vec4 myColor;

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
uniform sampler2D _MaterialTex;

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

vec3 calcPointLight(Light light, vec3 normal, vec3 pos, vec4 mat)
{

	

	vec3 lightDir = normalize(light.pos - pos);
	vec3 viewDir = normalize(_EyePos - pos);
	vec3 halfWay = normalize(lightDir + viewDir);

	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	
	vec3 diffuse = mat.g * diff * light.color;


	//specular
	float specStr = 0.5;
	float spec = pow(max(dot(normal, halfWay),0.0),mat.a);
	vec3 specColor = mat.b * spec * light.color;

	vec3 ambient = light.color * mat.r;

	return (diffuse + specColor) * attenuateLinear(length(light.pos - pos),light.radius);


//	vec3 diff = light.pos - pos;
//	//Direction toward light position
//	vec3 toLight = normalize(diff);
//	//TODO: Usual blinn-phong calculations for diffuse + specular
//	float diffuseFactor = max(dot(normal,toLight),0.0);
//
//	vec4 mat = texture(_MaterialTex, UV).rgba;
//
//	vec3 diffuseColor = light.color * mat.g;
//
//	vec3 toEye = normalize(_EyePos - pos);
//
//	vec3 h = normalize(toLight + toEye);
//
//	float specularFactor = pow(max(dot(normal,h),0.0),mat.a);
//
//	vec3 lightColor = (diffuseColor  + specularFactor * mat.b) * light.color;
//
//	//Attenuation
//	float d = length(diff); //Distance to light
//	lightColor *= attenuateLinear(d,light.radius); //See below for attenuation options
//	return lightColor;
}
//Linear falloff

//TODO:Add ambient light -- somthing might be wrong with the specular?
void main()
{
	vec2 UV = gl_FragCoord.xy / textureSize(_normals,0);

	vec3 normal = texture(_normals, UV).rgb;
	normal = normalize(normal);
	vec3 worldPos = texture(_positions, UV).rgb;

	vec4 mat = texture(_MaterialTex, UV).rgba;

	Light light = _lights;
	vec3 lightColor = calcPointLight(light, normal, worldPos, mat);

	myColor = vec4(lightColor * (texture(_albito, UV).rgb * mat.r), 1.0);
	//myColor = vec4(vec3(1.0,1.0,1.0), 1.0);

}