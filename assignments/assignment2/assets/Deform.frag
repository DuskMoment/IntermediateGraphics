#version 450

out vec4 FragColor; //the color of the fragment Shader

in Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
	mat3 TBN;
	vec4 fragPosLightSpace;

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
uniform sampler2D _ShadowMap;

//light uniforms
uniform vec3 _LightDirection = vec3(0.0, -1.0, 0.0);
uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);

//camera uniforms
uniform vec3 _EyePos;

uniform float _Bias;
uniform bool _PCF;

float shadowCalcualtion(vec4 fragPosLightSpace)
{

//	if(_PCF)
//	{
//	}
//	else
//	{
//	}

    vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;

	projCoords = projCoords * 0.5 + 0.5;

	float closestDepth = texture(_ShadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	if(projCoords.z  <= 0.0 || projCoords.z > 1.0)
	 {
		return 0.0;
	 }

	float shadow = 0.0;
	vec2 texelSize = 1.0/textureSize(_ShadowMap, 0);

	for(int x = -1; x <=1; ++x)
	{
		for(int y = -1; y<= 1; ++y)
		{
			float pcfDepth = texture(_ShadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += currentDepth - _Bias > pcfDepth ? 1.0 : 0.0;
		}
	}


	//float shadow = currentDepth  - _Bias > closestDepth ? 1.0 : 0.0;

	return shadow /= 9.0;
}

vec3 blinFong()
{
	
	vec3 normal = normalize(fs_in.WorldNormal);
	//normal = texture(_NormalMap, fs_in.TexCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	vec3 toLight = _LightDirection;
	//toLight = fs_in.TBN  * -toLight;

	toLight = -toLight;

	//defuse lighting
	float diffuseFactor = max(dot(normal,toLight),0.0);

	vec3 diffuseColor = _LightColor * diffuseFactor;

	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);

	//blinn phong
	vec3 h = normalize(toLight + toEye);
	
	//specular
	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);

	//sumation equation
	vec3 lightColor = (_Material.Kd * diffuseColor + _Material.Ks * specularFactor) * _LightColor;

	//vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

	return lightColor;
}
void main()
{
	
	vec3 lightColor = blinFong();

	vec3 objectColor = texture(_MainTex,fs_in.TexCoord).rgb;

	float shadow = shadowCalcualtion(fs_in.fragPosLightSpace);

	//_Light.color * ((_Material.ambientK + (1.0 - shadow) * lighting) * objectColor);

	vec3 finalColor = ((_AmbientColor * _Material.Ka) + (1.0 - shadow) * lightColor )* objectColor;

	
	FragColor = vec4(finalColor, 1.0);
	//FragColor = vec4(0.0);
}