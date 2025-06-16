#version 330 core
uniform bool useTexture;

out vec4 FragColor;

uniform vec3 uColor;

out vec4 outCol;

uniform sampler2D uTex; 
in vec2 chTex;



void main() {

   
        FragColor = vec4(uColor, 1.0);
    
  
}
