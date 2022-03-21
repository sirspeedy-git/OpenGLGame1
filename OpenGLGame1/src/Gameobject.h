#pragma once

#include <GL/glew.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "Mesh.h"

class Transform {
public:
	glm::vec3 Position = glm::vec3(0);
	glm::vec3 Rotation = glm::vec3(0);
	glm::vec3 Scale = glm::vec3(1);
};

class Gameobject {

public:
	Transform modelTransform;

	//Gameobject(Mesh objectMesh, Shader objectShader);
	//Gameobject(Mesh objectMesh, Shader objectShader, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
	~Gameobject();

	void Render();
	void Update();
	
private:
	Shader shader;
	Mesh mesh;
	glm::mat4 model;
};