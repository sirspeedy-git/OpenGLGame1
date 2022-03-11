#include "PhysicsWorld.h"

void PhysicsWorld::AddObject(Object* obj){
	objects.push_back(obj);
}

void PhysicsWorld::RemoveObject(Object* obj){
	if (!obj) return;
	auto itr = std::find(objects.begin(), objects.end(), obj);
	if (itr == objects.end()) return;
	objects.erase(itr);
}

void PhysicsWorld::Step(float dt) {
	for (Object* obj : objects) {
		obj->force += obj->mass * gravity;

		obj->vel += obj->force / obj->mass * dt;
		obj->pos += obj->vel * dt;

		obj->force = glm::vec3(0);
	}
}
