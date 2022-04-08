#include "Gameobject.h"

Transform::Transform() {
	this->posisiton = glm::vec3(0);
	this->rotation = glm::vec3(0);
	this->scale = glm::vec3(1);
}

Transform::Transform(glm::vec3 posistion, glm::vec3 rotation, glm::vec3 scale) {
	this->posisiton = posistion;
	this->rotation = rotation;
	this->scale = scale;
}

GameObject::GameObject(Mesh* mesh, Shader* shader){
	this->mesh = mesh;
	this->shader = shader;
	this->transform.posisiton = glm::vec3(0);
	this->transform.rotation = glm::vec3(0);
	this->transform.scale = glm::vec3(1);
	this->color = glm::vec3(1, 0, 1);
	this->transform.model = glm::mat4(1.0f);
	Update(0);
}

GameObject::GameObject(Mesh* mesh, Shader* shader, glm::vec3 posistion, glm::vec3 rotation, glm::vec3 scale, glm::vec3 color){
	this->mesh = mesh;
	this->shader = shader;
	this->transform.posisiton = posistion;
	this->transform.rotation = rotation;
	this->transform.scale = scale;
	this->color = color;
	this->transform.model = glm::mat4(1.0f);
	Update(0);
}

GameObject::GameObject(Mesh* mesh, Shader* shader, Transform transform, glm::vec3 color){
	this->mesh = mesh;
	this->shader = shader;
	this->transform.posisiton = transform.posisiton;
	this->transform.rotation = transform.rotation;
	this->transform.scale = transform.scale;
	this->color = color;
	this->transform.model = glm::mat4(1.0f);
	Update(0);
}


GameObject::~GameObject(){

}

void GameObject::Render(){
	shader->use();
	shader->setMat4("model", transform.model);
	shader->setVec3("objectColor", color);
	mesh->Draw();
}

void GameObject::Update(float dt){
	UpdateModelMatrix();
}

void GameObject::setPosistion(glm::vec3 posistion){
	this->transform.posisiton = posistion;
}

void GameObject::setRotation(glm::vec3 rotation){
	this->transform.rotation = rotation;
}

void GameObject::setScale(glm::vec3 scale){
	this->transform.scale = scale;
}

void GameObject::setColor(glm::vec3 color){
	this->color = color;
}

glm::vec3 GameObject::getPosistion()
{
	return transform.posisiton;
}

glm::vec3 GameObject::getRotation()
{
	return transform.rotation;
}

glm::vec3 GameObject::getScale()
{
	return transform.scale;
}

void GameObject::UpdateModelMatrix(){
	transform.model = glm::mat4(1.0f);
	transform.model = glm::translate(transform.model, transform.posisiton);
	transform.model = glm::rotate(transform.model, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
	transform.model = glm::rotate(transform.model, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
	transform.model = glm::rotate(transform.model, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
	transform.model = glm::scale(transform.model, transform.scale);
}
