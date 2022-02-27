#version 450 core

out vec4 fragColour;

uniform vec3 uColour;

in float type;


void main() { 


	fragColour = vec4(type / 128.0, type / 256.0, type / 512.0, 1.0);
}