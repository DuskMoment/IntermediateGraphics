#version 450

out vec4 FragColor;
in vec2 vs_texCoords; //the color of the fragment Shader

uniform sampler2D tex;

const float offset = 1.0/300.0;
const vec2 offsets[9] = vec2[]
(
	vec2(-offset, offset),//topleft
	vec2(0.0,offset),//top-center
	vec2(offset,offset),//top-right

	vec2(-offset,0.0),//center -left
	vec2(0.0,0.0),//center -center
	vec2(offset,0.0),//center -right

	vec2(-offset,-offset),//bottom - left
	vec2(0.0,-offset),//bottem -center
	vec2(offset,-offset)//bootm -  right
);
const float strenght = 9.0;
const float kernal[9] = float[](
1.0,1.0,1.0,
1.0,1.0,1.0,
1.0,1.0,1.0
);

uniform float intensity;
void main()
{
	vec3 avr = vec3(0.0);

	for(int i = 0; i <9;i++)
	{
	 vec3 local = texture(tex ,vs_texCoords + offsets[i]).rgb;
	 avr += local * (kernal[i]/strenght);
	}

	vec3 ablito = texture(tex, vs_texCoords).rgb;


	FragColor = vec4(vec3(avr), 1.0);

}