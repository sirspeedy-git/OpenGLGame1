#pragma once

#include <GL/glew.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "Mesh.h"

class GameObject {
public:
	GameObject(Mesh* mesh, Shader* shader);
	GameObject(Mesh* mesh, Shader* shader, glm::vec3 posistion, glm::vec3 rotation, glm::vec3 scale);
	~GameObject();

	void Render();
	void Update();
	void setPosistion(glm::vec3 posistion);
	void setRotation();
	void setScale();
	void setColor(glm::vec3 color);

private:

	void UpdateModelMatrix();

	glm::vec3 pos;
	glm::vec3 Rot;
	glm::vec3 Scale;
	glm::vec3 color;
	glm::mat4 model;
	Mesh* mesh;
	Shader* shader;
	//unsigned int ID;
};