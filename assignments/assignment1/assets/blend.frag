#version 450

out vec4 FragColor;
in vec2 vs_texCoords; //the color of the fragment Shader

uniform sampler2D tex;
uniform sampler2D bloomBlur;

uniform float exposure;

void main()
{
	const float gamma = 2.2;

	vec3 hdrColor = texture(tex, vs_texCoords).rgb;
	vec3 bloomColor = texture(bloomBlur, vs_texCoords).rgb;

	hdrColor +=  bloomColor;

	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

	result = pow(result, vec3(gamma));

	FragColor = vec4(result, 1.0);

}