#version 450

out vec4 FragColor;
in vec2 vs_texCoords; //the color of the fragment Shader

uniform sampler2D tex;

uniform float intesity;

void main()
{
	vec2 location = vs_texCoords - 0.5;
	float dist = length(location);

	vec3 color = texture(tex, vs_texCoords).rgb;

	color *= (1 - dist * intesity);

	FragColor = vec4(color, 1.0);

}