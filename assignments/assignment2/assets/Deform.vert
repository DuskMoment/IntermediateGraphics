#version 450

//vertex attributes
layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoords;
layout(location = 3) in vec3 in_Tangent;

uniform mat4 _Model;
uniform mat4 _VeiwProjection;
uniform mat4 _LightSpaceMatrix;

out Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
	mat3 TBN;
	vec4 fragPosLightSpace;
}vs_out;

uniform sampler2D _ImprintMap;

float shadowCalcualtion(vec4 fragPosLightSpace)
{

//	if(_PCF)
//	{
//	}
//	else
//	{
//	}

    vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;

	projCoords = projCoords * 0.5 + 0.5;

	float closestDepth = texture(_ImprintMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	if(projCoords.z  <= 0.0 || projCoords.z > 1.0)
	 {
		return 0.0;
	 }

	float shadow = 0.0;
	vec2 texelSize = 1.0/textureSize(_ImprintMap, 0);

	for(int x = -1; x <=1; ++x)
	{
		for(int y = -1; y<= 1; ++y)
		{
			float pcfDepth = texture(_ImprintMap, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += currentDepth - 00.5 > pcfDepth ? 1.0 : 0.0;
		}
	}


	//float shadow = currentDepth  - _Bias > closestDepth ? 1.0 : 0.0;

	return shadow /= 9.0;
}

void main()
{
	//model to world space converstions
	vs_out.WorldPos = vec3(_Model * vec4(in_Pos,1.0));
	vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * in_Normal;

	vec3 T = normalize(vec3(_Model * vec4(in_Tangent, 0.0)));
	vec3 N = normalize(vec3(_Model * vec4(in_Normal, 0.0)));

	//re-orthogonalize T with respect to N 
	T = normalize(T - dot(T,N) * N);

	//vec3 B = normalize(vec3(_Model * vec4(in_biTangnet, 0.0)));
	vec3 B = cross(N,T);

	vs_out.TBN = transpose(mat3(T,B,N));
	//vs_out.TBN = mat3(T,B,N);


	//mesh deformation
	vs_out.fragPosLightSpace = _LightSpaceMatrix * vec4(vs_out.WorldPos, 1.0);
	vec3 projCoords = vs_out.fragPosLightSpace.xyz/vs_out.fragPosLightSpace.w;


	projCoords = projCoords * 0.5 + 0.5;

	float map = (texture(_ImprintMap, projCoords.xy).r - 1) * -1;

	vec3 new_pos = in_Pos - vec3(0, map * 0.2f, 0);
	//texture
	vs_out.TexCoord = in_TexCoords;

	vec4 final = (_VeiwProjection * _Model * vec4(new_pos, 1.0));

	vs_out.WorldPos = vec3(_Model * vec4(new_pos,1.0));
	
	gl_Position = final;
}