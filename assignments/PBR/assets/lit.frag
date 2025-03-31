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

//camera uniforms
uniform vec3 _EyePos;

void main()
{
	//load normal map
	
	vec3 normal = normalize(fs_in.WorldNormal);
	normal = texture(_NormalMap, fs_in.TexCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	vec3 toLight = _LightDirection;
	toLight = fs_in.TBN  * -toLight;

	//defuse lighting
	float diffuseFactor = max(dot(normal,toLight),0.0);

	vec3 toEye = fs_in.TBN * normalize(_EyePos - fs_in.WorldPos);

	//blinn phong
	vec3 h = normalize(toLight + toEye);
	
	//specular
	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);

	//sumation equation
	vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;
	lightColor += _AmbientColor * _Material.Ka;

	vec3 objectColor = texture(_MainTex,fs_in.TexCoord).rgb;

	FragColor = vec4(objectColor * lightColor, 1.0);
	//FragColor = vec4(0.0);
}