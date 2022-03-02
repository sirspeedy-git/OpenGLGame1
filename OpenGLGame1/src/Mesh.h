#pragma once

#include <vector>
#include "glm/glm/glm.hpp"
#include "Shader.h"

class Vertex {
public:
	glm::vec3 Posisiton;
	glm::vec3 Normal;
	glm::vec2 UV;
};

class Mesh {
public:

	std::vector<Vertex> vertecies;
	std::vector<unsigned int> indices;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	void Draw(Shader &shader);
private:

	unsigned int VAO, VBO, EBO;
	void SetupMesh();
};