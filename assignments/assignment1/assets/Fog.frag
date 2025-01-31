#version 450

out vec4 FragColor;
in vec2 vs_texCoords; //the color of the fragment Shader

uniform sampler2D tex;

const vec3 offset = vec3(0.009,0.006,-0.006);
const vec2 direction = vec2(1.0);

float near = 0.1;
float far = 10.0;


float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    vec4 depthVec4 = vec4(vec3(pow(depth, 1.4)), 1.0);
    vec4 color = texture(tex, vs_texCoords);
    //color +=depth;
    //FragColor = color * (1-depthVec4) + depthVec4; 
      FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}