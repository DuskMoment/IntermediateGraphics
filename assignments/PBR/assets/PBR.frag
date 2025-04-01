#version 450
layout(location = 0) out vec4 myColor;

in Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
	mat3 TBN;
}fs_in;

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

uniform float metalic = 1.0;
uniform float roughness = 1.0;

//chace dot producs 

float vDotn = 0;
float lDotn = 0;
float nDoth = 0;


float attenuateLinear(float dist, float radius){
	return clamp((radius-dist)/radius,0.0,1.0);
}

vec3 PBR(Light light, vec3 normal, vec3 pos, vec2 UV)
{

	vec3 diff = light.pos - pos;
	//Direction toward light position
	vec3 toLight = normalize(diff);
	//TODO: Usual blinn-phong calculations for diffuse + specular
	float diffuseFactor = max(dot(normal,toLight),0.0);

	vec4 mat = texture(_MaterialTex, UV).rgba;

	vec3 diffuseColor = light.color * mat.g;

	vec3 toEye = normalize(_EyePos - pos);

	vec3 h = normalize(toLight + toEye);

	float specularFactor = pow(max(dot(normal,h),0.0),mat.a);

	vec3 lightColor = (diffuseColor  + specularFactor * mat.b) * light.color;

	//Attenuation
	float d = length(diff); //Distance to light
	lightColor *= attenuateLinear(d,light.radius); //See below for attenuation options
	return vec3(1.0,0,0);
}

const float PI = 3.14159264;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

//crazy crazy functions
vec3 G()
{
	
	return vec3(1.0);
	
}
vec3 D()
{	
	//alpha = roughness ^ 2
	float a2 = pow(roughness, 4.0);

	float denom = PI * pow(pow(nDoth, 2.0) * (a2 - 1.0) + 1.0, 2.0);

	return vec3(1.0);
}

//describe specualr
vec3 cookTorrence(vec3 fresnel)
{
	vec3 numerator  = D() * fresnel * G();
	float denomonator = 4.0 * vDotn * lDotn;

	denomonator = max(denomonator, 0.001);

	return numerator/denomonator;

}
vec3 BDRF(vec3 pos, vec3 in_direction)
{
    //endergy conservation
	//vec3 diffuse = vec3(1.0);
	//vec3 specualr = vec3(1.0);

	vec3 lambert = _lights.color/PI;
	//view dot half
	vec3 toEye = normalize(_EyePos - pos);

	vec3 diff = _lights.pos - pos;
	//Direction toward light position
	vec3 toLight = normalize(diff);

	vec3 h = normalize(in_direction + toEye);

	//fresnelSchlick(dot(toEye, h),lambert)

	vec3 Ks = fresnelSchlick(dot(toEye, h), lambert);

	vec3 Kd = 1 - Ks * (1.0 - metalic);

	vec3 diffuse = (Kd * lambert);
	vec3 specualr = Ks * cookTorrence(Ks);

	return diffuse + specualr;
	
	//return vec3(1.0, 0.0, 0.0);
}

//Linear falloff

//TODO:Add ambient light -- somthing might be wrong with the specular?
void main()
{
	vec2 UV = gl_FragCoord.xy / textureSize(_normals,0);

	vec3 normal = texture(_normals, UV).rgb;
	vec3 worldPos = texture(_positions, UV).rgb;

	normal = normalize(normal);

	lDotn = dot(normal, normalize(_lights.pos - worldPos));

	vDotn = max(dot(normalize(_EyePos - worldPos), normal), 0);

	vec3 h = normalize(normalize(_EyePos - _lights.pos));

	h = normalize(h + normalize(_lights.pos - worldPos));

	nDoth = max(dot(h, normal),0);

	Light light = _lights;
	//vec3 lightColor = PBR(light ,vec3(0), vec3(0), vec2(0)) * attenuateLinear(length(light.pos - worldPos),light.radius) * vec3(1.0,0.0,0.0);

	vec3 final = BDRF(worldPos, normalize(light.pos - worldPos)) * attenuateLinear(length(light.pos - worldPos),light.radius) * light.color * lDotn;

	//myColor = vec4(lightColor * texture(_albito, UV).rgb, 1.0);
	//myColor = vec4(vec3(1.0,1.0,1.0), 1.0);
	myColor = vec4(final, 1.0);

	

}



	