#version 450

out vec4 FragColor;
in vec2 vs_texCoords; //the color of the fragment Shader

uniform sampler2D tex;

void main()
{
	vec3 ablito = 1.0 - texture(tex, vs_texCoords).rgb;

	FragColor = vec4(ablito, 1.0);

}