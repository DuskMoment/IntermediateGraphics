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

//shadow uniforms
uniform float _Bias = 0.005;
uniform float _BiasMax = 0.05;
uniform int _PCF;
uniform int _PCFAmmount = 1;


float shadowCalcualtion(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	//get correct coordinates
    vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	float bias = max(_BiasMax * (1.0 - dot(normal, lightDir)), _Bias);  

	//sample texture
	float closestDepth = texture(_ShadowMap, projCoords.xy).r;
	if(_PCF == 0)
	{
		float currentDepth = projCoords.z;

		//top the z axis issue
		if(projCoords.z  <= 0.0 || projCoords.z > 1.0)
		 {
			return 0.0;
		 }

		 float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

		 return shadow;
	}
	

	float shadow = 0.0;
	vec2 texelSize = 1.0/textureSize(_ShadowMap, 0);
	float currentDepth = projCoords.z;

	for(int x = -1; x <= _PCFAmmount; ++x)
	{
		for(int y = -1; y<= _PCFAmmount; ++y)
		{
			float pcfDepth = texture(_ShadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += currentDepth - _Bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	
	return shadow /= 9.0;
}

vec3 blinFong()
{
	vec3 normal = normalize(fs_in.WorldNormal);

	vec3 toLight = normalize(_LightDirection - fs_in.WorldPos);

	float diffuseFactor = max(dot(normal,toLight),0.0);

	vec3 diffuseColor = _LightColor * diffuseFactor;

	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 h = normalize(toLight + toEye);

	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);

	vec3 lightColor = (diffuseColor * _Material.Kd + specularFactor * _Material.Ks) * _LightColor;


	return lightColor;

}
void main()
{
	
	vec3 lightColor = blinFong();

	vec3 objectColor = texture(_MainTex,fs_in.TexCoord).rgb;

	float shadow = shadowCalcualtion(fs_in.fragPosLightSpace, normalize(fs_in.WorldNormal), normalize(_LightDirection - fs_in.WorldPos));

	//_Light.color * ((_Material.ambientK + (1.0 - shadow) * lighting) * objectColor);

	vec3 finalColor = ((_AmbientColor * _Material.Ka) + (1.0 - shadow) * lightColor ) * objectColor;

	FragColor = vec4(finalColor * objectColor, 1.0);
}