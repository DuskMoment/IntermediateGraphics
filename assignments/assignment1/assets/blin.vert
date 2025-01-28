#version 450

//vertex attributes
layout(location = 0) in vec2 in_Pos;
layout(location = 1) in vec2 in_Texcoords;


out vec2 vs_texCoords;

void main()
{

	vs_texCoords = in_Texcoords;
	
	gl_Position = vec4(in_Pos.xy, 0.0,1.0);	
}