#version 450

out vec4 FragColor;

uniform vec3 camera_pos;
uniform vec3 water_color;
in vec2 vs_TexCoords;

in vec3 to_cam;
const vec3 reflectColor = vec3(1.0f,0.0f,0.0f);

uniform sampler2D waterTex;
uniform sampler2D warpTex;
uniform sampler2D specTex;


uniform float tile;
uniform float time;

uniform float b1;
uniform float b2;


uniform float scale;
uniform float warp_scale;
uniform float spec_scale;

uniform float bright_lower_cutoff;
uniform float bright_upper_cutoff;
float warpStregth = 2;

void main()
{

	vec2 uv = vs_TexCoords;
	//warp
	vec2 warp_uv = vs_TexCoords * warp_scale;
	vec2 warp_scroll = vec2(0.5, 0.5) * time;
	vec2 warp = texture(warpTex, warp_uv + warp_scroll).rb * warpStregth;
	warp = warp * (2.0 -1.0);

	vec2 albito_uv = vs_TexCoords * scale;
	vec2 albito_scroll = vec2(-0.5,0.5) * time;
	vec4 albito = texture(waterTex, albito_uv + warp + albito_scroll);

	//specualr
	vec2 spec_uv = vs_TexCoords * spec_scale;
	vec3 smp1 = texture(specTex, spec_uv + vec2(1.0,0.0) + time).rgb;
	vec3 smp2 = texture(specTex, spec_uv + vec2(1.0,1.0) + time).rgb;

	vec3 spec = (smp1 + smp2);
	
	float bright = dot(spec, vec3(0.299, 0.587, 0.114));

//	if(bright <= bright_lower_cutoff || bright >= bright_upper_cutoff)
//	{
//		discard;
//
//	}

	float fresnel = dot(normalize(to_cam), vec3(0.0,1.0,0.0));

	vec3 finalColor = mix(reflectColor, water_color + vec3(albito.r), fresnel);



	FragColor = vec4(finalColor, 1.0f);
}