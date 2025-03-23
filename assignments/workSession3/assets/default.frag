#version 450
//out vec4 FragColor; //the color of the fragment Shader
layout(location = 0)out vec4 FragAlbito;
layout(location = 1)out vec4 FragPos;
layout(location = 2)out vec4 FragNormal;
in Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
	mat3 TBN;
}fs_in;

//camera uniforms
uniform vec3 _EyePos;
uniform vec3 _Color = vec3(1.0,1.0,1.0);



void main()
{

	//load normal map
	//FragColor = vec4(1.0, 0, 0, 1.0);
	//FragColor = vec4(fs_in.WorldPos.xyz, 1.0);

	FragAlbito = vec4(_Color,1.0); 
	FragPos = vec4(fs_in.WorldPos, 1.0);
	FragNormal = vec4(fs_in.WorldNormal.xyz, 1.0);
}