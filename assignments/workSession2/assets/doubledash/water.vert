#version 450

//vertex attributes
layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoords;

uniform mat4 model;
uniform mat4 view_proj;
//uniform vec3 camera_proj;
uniform vec3 camera_pos;

uniform float time;
uniform float strenght;
out vec2 vs_TexCoords;
out vec3 to_cam;

float calculateSurface(float x, float z) {
  float scale = 10.0;
  float y = 0.0;
  y += (sin(x * 1.0 / scale + time * 1.0) + sin(x * 2.3 / scale + time * 1.5) + sin(x * 3.3 / scale + time * 0.4)) / 3.0;
  y += (sin(z * 0.2 / scale + time * 1.8) + sin(z * 1.8 / scale + time * 1.8) + sin(z * 2.8 / scale + time * 0.8)) / 3.0;
  return y;
}
void main()
{	
	//vec3 pos = in_Pos;

	//pos += calculateSurface(in_Pos.x, in_Pos.z) * strenght;
	vec4 worldPos = model * vec4(in_Pos, 1.0);

	vs_TexCoords = in_TexCoords;

	to_cam = camera_pos - worldPos.xyz;

	gl_Position = view_proj * model * vec4(in_Pos, 1.0f);

}

