#version 450
//source --> https://medium.com/@vlh2604/real-time-radial-and-tangential-lens-distortion-with-opengl-a55b7493e207
out vec4 FragColor;
in vec2 vs_texCoords; //the color of the fragment Shader

uniform sampler2D tex;

uniform float intesity;

uniform vec3 radialDefault;
uniform vec2 tangentDefault;

vec2 radialDistorition(vec2 coord, float k1, float k2, float k3)
{
	float r = length(coord);
	float distortionFactor = 1.0 + k1 * pow(r, 2) + k2 * pow(r,4) + k3 * pow(r,6);
	return distortionFactor * coord;
}

vec2 tangentalDistortion(vec2 coord, float p1, float p2)
{
	float x = coord.x;
	float y = coord.y;
	float r2 = x*x + y*y;
	float dx = 2.0 * p1 * x * y + p2 * (r2 + 2.0 * x * x);
	float dy = p1 * (r2 + 2.0 * y * y) + 2.0 * p2 * x * y;
	return vec2(dx, dy);
}

void main()
{
	
	vec2 coords = vs_texCoords * 2.0 - 1.0;

	vec2 radDis = radialDistorition(coords,radialDefault.x,radialDefault.y,radialDefault.z);

	vec2 tanDis = tangentalDistortion(coords, tangentDefault.x,tangentDefault.y);
	
	vec2 distCoords = radDis + tanDis;

	distCoords = (distCoords + 1.0) / 2.0;
	
	vec3 color = texture(tex, distCoords).rgb;


	FragColor = vec4(color, 1.0);

}