#version 450 core

layout (location = 0) in vec4 pos;

out float type;

uniform mat4 MVP;

void main() { 
	type = pos.w;
	gl_Position = MVP * vec4(pos.xyz, 1.0);
}