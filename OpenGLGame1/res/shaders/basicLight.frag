#version 450 core

in vec3 fragPos;
in vec2 texCoord0;
in vec3 normal0;

uniform sampler2D sampler;
uniform vec3 lightPos;

vec3 lightCol = vec3(1,1,1);

void main()
{
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightCol;

	vec3 norm = normalize(normal0);
	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightCol;

	vec3 result = (ambient + diffuse) * texture2D(sampler, texCoord0).xyz;

	gl_FragColor = vec4(result, 1.0);
}