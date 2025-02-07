#version 450

out vec4 FragColor;
in vec2 vs_texCoords; //the color of the fragment Shader

uniform sampler2D tex;

uniform float offsetR;
uniform float offsetG;
uniform float offsetB;

const vec3 offset = vec3(offsetR, offsetG, offsetB);
const vec2 direction = vec2(1.0);

void main()
{

	vec3 ablito = texture(tex, vs_texCoords).rgb;
	FragColor.r = texture(tex,vs_texCoords + (direction * vec2(offset.r))).r;
	FragColor.g = texture(tex,vs_texCoords + (direction * vec2(offset.g))).g;
	FragColor.b = texture(tex,vs_texCoords + (direction * vec2(offset.b))).b;
	FragColor.a = 1.0;

	//FragColor = vec4(vec3(avr), 1.0);

}