#version 450

out vec4 FragColor;


//uniforms
uniform vec3 cam_pos;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform sampler2D shadow_map;
uniform mat4 _lightVeiwProj;

in vec4 light_pos_in;
struct Material 
{
	float Ka;
	float Kd;
	float Ks;
	float Shininess;
};

uniform Material material;

//in
in vec3 vs_normal;
in vec3 vs_position;
in vec3 vs_TexCoords;

struct Light
{
	vec3 position;
	vec3 color;
};

uniform Light light;

in Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
	mat3 TBN;
}vs_out;

float shadow_calculation(vec4 frag_pos_lightspace)
{
	//prespecitve divide
	vec3 proj_cords = frag_pos_lightspace.xyz /frag_pos_lightspace.w;
	//put it on range (0,1) good for texture sampling
	 proj_cords = (proj_cords * 0.5) + 0.5;

	 float lightDepth = texture(shadow_map, proj_cords.xy).r;
	 float cameraDepth = proj_cords.z;

	 if(proj_cords.z  = <0.0 || proj_cords.z > 1.0)
	 {
		return 0.0;
	 {

	 float shadow = lightDepth >  cameraDepth ? 1.0 : 0.0;

	 return shadow;
}
vec3 blinfong(vec3 normal, vec3 frag_pos)
{
	vec3 view_dir = normalize(cam_pos - frag_pos);
	vec3 light_dir = normalize(light_pos - frag_pos);
	vec3 halfway_dir = normalize(light_dir + view_dir);

	float ndot1 = max(dot(normal, light_dir),0.0);
	float ndoth = max(dot(normal, halfway_dir),0.0);

	vec3 diffuse = vec3(ndot1 + material.Kd);
	vec3 sepc = vec3(pow(ndot1, material.Shininess  * 128.0) + material.Ks);

	return diffuse + sepc;
}
void main()
{
	vec3 normal = normalize(vs_out.WorldNormal);
	vec3 lighting = blinfong(normal, vs_out.WorldNormal);

	lighting *= light_color;
	float shadow = shadow_calculation(light_pos_in);
	 
	vec3 object_color = normal * 0.5 + 0.5;

	FragColor = vec4(object_color * light_color, 1.0);

}