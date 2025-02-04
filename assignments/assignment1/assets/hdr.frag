#version 450

out vec4 FragColor;
in vec2 vs_texCoords; //the color of the fragment Shader

uniform sampler2D tex;

uniform sampler2D hdrBuffer;

uniform float exposure;

void main()
{
	
	const float gamma = 2.2;

	vec3 hdrColor = texture(tex, vs_texCoords).rgb;

	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

	mapped = pow(mapped, vec3(1.0/gamma));

	FragColor = vec4(mapped, 1.0);

}