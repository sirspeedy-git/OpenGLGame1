#version 450 core

in vec2 texCoord0;

uniform sampler2D albedoMap;

void main()
{

	gl_FragColor = texture2D(albedoMap, texCoord0);
}