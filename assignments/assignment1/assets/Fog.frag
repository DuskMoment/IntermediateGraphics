#version 450

out vec4 FragColor;
in vec2 vs_texCoords; //the color of the fragment Shader

uniform sampler2D tex;
uniform sampler2D depthTex;

const vec3 offset = vec3(0.009,0.006,-0.006);
const vec2 direction = vec2(1.0);

float near = 0.1; 
float far  = 100.0; 

vec3 fogColor = vec3(1.0);

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}
float logisticDepth(float depth, float stepness = 0.5, float lOffset = 5.0)
{
    float zVal = LinearizeDepth(depth);
    return (1/ (1+exp(-stepness * (zVal - lOffset))));
}
void main()
{   
    float depth = logisticDepth(texture(depthTex, vs_texCoords).r, 0.7, 25.0);
    vec3 color = mix(texture(tex, vs_texCoords).rgb, fogColor, depth);
    
    FragColor = vec4(color, 1.0);
 }
   