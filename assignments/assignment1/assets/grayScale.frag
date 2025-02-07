#version 450

out vec4 FragColor;
in vec2 vs_texCoords; //the color of the fragment Shader

uniform sampler2D tex;

uniform float intensity;

void main()
{
	vec3 ablito = texture(tex, vs_texCoords).rgb;
	//naive
	//float avr = (ablito.r + ablito.g + ablito.b)/3;
	float avr = ((ablito.r *0.2161)+ (ablito.g * 0.7152)+ (ablito.b * 0.0722))/3;
	FragColor = vec4(vec3(avr) * intensity, 1.0);

}