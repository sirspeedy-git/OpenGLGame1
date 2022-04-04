#pragma once

#include <GL/glew.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "Mesh.h"

class Transform {
public:

	Transform();
	Transform(glm::vec3 posistion, glm::vec3 rotation, glm::vec3 scale);

	glm::vec3 posisiton;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4 model = glm::mat4(0);
};

class GameObject {
public:
	GameObject(Mesh* mesh, Shader* shader);
	GameObject(Mesh* mesh, Shader* shader, glm::vec3 posistion, glm::vec3 rotation, glm::vec3 scale, glm::vec3 color);
	GameObject(Mesh* mesh, Shader* shader, Transform transform, glm::vec3 color);
	~GameObject();

	void Render();
	void Update(float dt);
	void setPosistion(glm::vec3 posistion);
	void setRotation(glm::vec3 rotation);
	void setScale(glm::vec3 scale);
	void setColor(glm::vec3 color);

	glm::vec3 getPosistion();
	glm::vec3 getRotation();
	glm::vec3 getScale();

protected:

	void UpdateModelMatrix();


	Transform transform;

	glm::vec3 color;

	Mesh* mesh;
	Shader* shader;
	//unsigned int ID;
};