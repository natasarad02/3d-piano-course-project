#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inCol;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 aTex; //Koordinate texture, propustamo ih u FS kao boje
out vec2 chTex;

uniform mat4 uM; //Matrica transformacije
uniform mat4 uV; //Matrica kamere
uniform mat4 uP; //Matrica projekcija

out vec4 channelCol;
out vec3 FragPos;
out vec3 Normal;

void main()
{
	vec4 worldPos = uM * vec4(inPos, 1.0);
    FragPos = vec3(worldPos);
    
 
    channelCol = inCol;
    
    gl_Position = uP * uV * worldPos;
    chTex = aTex;
}