#version 450

out vec4 FragColor;

uniform vec3 camera_pos;
uniform vec3 water_color;
in vec2 vs_TexCoords;

in vec3 to_cam;
const vec3 reflectColor = vec3(1.0f,0.0f,0.0f);

uniform sampler2D waterTex;

uniform float tile;
uniform float time;

uniform float b1;
uniform float b2;
void main()
{


	vec2 dir = normalize(vec2(1.0));
	vec2 uv = (vs_TexCoords*tile) + dir * time;

	uv.y += 0.01 * (sin(uv.x * 3.5 + time * 0.35) + sin(uv.x * 4.8 + time * 1.05) + sin(uv.x * 7.3 + time * 0.45)) / 3.0;
    uv.x += 0.12 * (sin(uv.y * 4.0 + time * 0.5) + sin(uv.y * 6.8 + time * 0.75) + sin(uv.y * 11.3 + time * 0.2)) / 3.0;
    uv.y += 0.12 * (sin(uv.x * 4.2 + time * 0.64) + sin(uv.x * 6.3 + time * 1.65) + sin(uv.x * 8.2 + time * 0.45)) / 3.0;


	float factor = dot(normalize(to_cam), vec3(0.0f,1.0f,0.0f));
	
	vec3 water = texture(waterTex, uv).rgb; 

	//vec3 color = mix(reflectColor, water_color, factor);

	//color = mix(color, water, 0.5);

	vec4 smp1 = texture(waterTex, uv);
	vec4 smp2 = texture(waterTex, uv + vec2(0.2));

	vec3 color = water_color + vec3(smp1.r * b1 - smp2.r * b2);

	FragColor = vec4(color, 1.0);
}