#version 450
// source --> https://www.shadertoy.com/view/3sGGRz
out vec4 FragColor;
in vec2 vs_texCoords; //the color of the fragment Shader

uniform sampler2D tex;

uniform float intesity;

uniform float time;

void main()
{
	float mdf = 1.0;
	float noise = (fract(sin(dot(vs_texCoords, vec2(12.9898,78.233)*2.0)) * 43758.5453));

	
	vec3 color = texture(tex, vs_texCoords).rgb;
	//scale by time
	mdf = mdf * (sin(time)) + 1.0;

	vec3 col = color - noise * intesity ;// * mdf;

	FragColor = vec4(col, 1.0);

}