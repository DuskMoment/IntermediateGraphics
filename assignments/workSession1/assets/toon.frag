#version 450

out vec4 FragColor; //the color of the fragment Shader

in Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
	//mat3 TBN;
}fs_in;

struct Material{
	float Ka;
	float Kd;
	float Ks;
	float Shininess;
	
};

struct Pallet
{
	vec3 highlight;
	vec3 shadow;
};

//uniforms
uniform Material _Material;
uniform Pallet _Pallet;

//texture uniforms
uniform sampler2D _MainTex;
uniform sampler2D zatoon;
//uniform sampler2D _NormalMap;

//light uniforms
uniform vec3 _LightDirection = vec3(0.0, 1.0,0.0);
uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);

uniform vec3 _Highlight;
uniform vec3 _Shadow;


//camera uniforms
uniform vec3 _EyePos;

vec3 toonLighting(vec3 normal, vec2 frag_pos, vec3 light_direction)
{
	float diff = (dot(normal, light_direction) + 1) * 0.5;
	vec3 light_color = vec3(1.0) * diff;

	float stepping = texture(zatoon, vec2(diff)).r;

	light_color = mix(_Pallet.shadow, _Pallet.highlight, stepping);
	return light_color * stepping;
}

void main()
{
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 lightColor = toonLighting(normal, fs_in.TexCoord, _LightDirection); 
	vec3 objectColor = texture(_MainTex, fs_in.TexCoord).rgb;
	FragColor = vec4( objectColor * lightColor, 1.0);
}