#version 450

out vec4 FragColor; //the color of the fragment Shader

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

//texture uniforms
uniform sampler2D _MainTex;
uniform sampler2D _NormalMap;

//light uniforms
uniform vec3 _LightDirection = vec3(0.0, -1.0,0.0);
uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);

uniform float _Roughness = 0.5;
//camera uniforms
uniform vec3 _EyePos;

//vars
const float PI = 3.14;
vec3 h;
vec3 normal;

float ggxDistrobution(float nDoth)
{
	float alpha2 = _Roughness * _Roughness * _Roughness * _Roughness;
	
	float d = nDoth * nDoth * (alpha2 - 1) + 1;

	return alpha2/(PI * d * d);
}

//do this one twice
float ggxSchlick(float dp)
{
	float k = (_Roughness + 1.0) * (_Roughness + 1.0)/8.0;
	float d = dp * (k-1) + k;

	return dp / d;
}

//add a bool for metals
vec3 Fresnel(float vDoth)
{
	vec3 F0 = vec3(0.04);
	return F0 + (1-F0) * pow(clamp(1.0 - vDoth, 0.0, 1.0),5); 
}

vec3 CalcPBR(vec3 view, vec3 l)
{
	vec3 Intensity  = _LightColor * 1;
	float nDoth = max(dot(normal,h), 0.0);
	float vDoth = max(dot(view,h), 0.0);
	float nDotl = max(dot(normal,l), 0.0);
	float nDotv = max(dot(normal,view), 0.0);

	vec3 F = Fresnel(vDoth);
	vec3 Ks = F;
	vec3 Kd = 1.0 - Ks;

	vec3 specNom = ggxDistrobution(nDotl) * F * ggxSchlick(nDotl) * ggxSchlick(nDotv);
	float specDenom = 4.0 * nDotv * nDotl + 0.0001;
	
	vec3 spec = specNom/specDenom;
	vec3 lambert = vec3(0);

	vec3 diffuse = Kd * lambert / PI;

	vec3 finalColor = (diffuse + spec) * Intensity * nDotl;

	return finalColor;


}
void main()
{
	//load normal map
	
	normal = normalize(fs_in.WorldNormal);
	//normal = texture(_NormalMap, fs_in.TexCoord).rgb;
	//normal = normalize(normal * 2.0 - 1.0);

	vec3 toLight = _LightDirection;
	//toLight = fs_in.TBN  * -toLight;
	toLight = -toLight;


	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);

	vec3 objectColor = texture(_MainTex,fs_in.TexCoord).rgb;

	h = normalize(toLight + toEye);
	vec3 color = CalcPBR(toEye, toLight);

	FragColor = vec4(color * objectColor, 1.0);
	//FragColor = vec4(0.0);
}