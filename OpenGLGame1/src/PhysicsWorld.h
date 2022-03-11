#pragma once

#include <vector>
#include "glm/glm/glm.hpp"

struct Object {
	glm::vec3 pos;
	glm::vec3 vel;
	glm::vec3 force;
	float mass;
	//Mesh mesh;
	//Shader shader;
};

class PhysicsWorld {
private:
	std::vector<Object*> objects;
	glm::vec3 gravity = glm::vec3(0, -9.8, 0);

public:
	void AddObject(Object* obj);
	void RemoveObject(Object* obj);
	void Step(float dt);
};