#include "Gameobject.h"

//Gameobject::Gameobject(Mesh objectMesh, Shader objectShader){
//	this->mesh = objectMesh;
//	this->shader = objectShader;
//	this->model = glm::mat4(1.0f);
//}
//
//Gameobject::Gameobject(Mesh* objectMesh, Shader objectShader, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale){
//	this->mesh = objectMesh;
//	this->shader = objectShader;
//	this->model = glm::mat4(1.0f);
//	modelTransform.Position = pos;
//	modelTransform.Rotation = rot;
//	modelTransform.Scale = scale;
//}

Gameobject::~Gameobject()
{
}

void Gameobject::Render(){
	shader.use();

	model = glm::mat4(1.0f);
	model = glm::translate(model, modelTransform.Position);
	model = glm::rotate(model, glm::radians(0.0f), modelTransform.Rotation);
	model = glm::scale(model, modelTransform.Scale);
	shader.setMat4("model", model);

	mesh.Draw();
}

void Gameobject::Update()
{
}
