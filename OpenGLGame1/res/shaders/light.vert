#version 450 core

layout(location = 0) in vec4 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;




void main() { 
	mat4 view_model = view * model;



	gl_Position = projection * view * model * vec4(aPos.xyz, 1.0);
}