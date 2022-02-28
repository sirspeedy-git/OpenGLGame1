#pragma once

#include <vector>
#include "glm/glm/glm.hpp"

class Vertex {
	glm::vec3 Posisiton;
	glm::vec3 Normal;
	glm::vec2 UV;
};

class Mesh {
public:

	std::vector<Vertex> vertecies;
	std::vector<unsigned int> indices;

	Mesh();
	void Draw();
private:

	unsigned int VAO, VBO, EBO;
	void SetupMesh();
};