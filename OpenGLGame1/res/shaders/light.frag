#version 450 core

out vec4 fragColour;

uniform vec3 col;
vec3 lightCol = vec3(1);

void main() { 
	float ambientStrenght = 0.1;
	vec3 ambient = ambientStrenght * vec3(1);

	vec3 result = ambient * col;
	fragColour = vec4(result, 1);
}