#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <chrono>
#include "Shader.h"
#include "Camera.h"
#include "stb_image.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "Mesh.h"
#include "Gameobject.h"

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* widow, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char *path);

int WIDTH = 1280, HEIGHT = 720;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 5.0f, 15.0f));
float lastX = WIDTH / 2;
float lastY = HEIGHT / 2;
bool firstMouse = true;

glm::vec3 lightDir(1.0f, 1.5f, 2.0f);

class Timer {
public:
	Timer() {
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer() {
		Stop();
	}

	void Stop() {
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		auto duration = end - start;
		double ms = duration * 0.001;

		std::cout << ms << "ms\n";
	}
private:
	std::chrono::time_point< std::chrono::high_resolution_clock> m_StartTimepoint;
};


//TODO - rigidbody phisics
//TODO - shadows
//TODO - model loading
//TODO - entitys

class Player : public GameObject {
public:
	
	Player(Mesh* mesh, Shader* shader, Camera* camera, glm::vec3 posistion, glm::vec3 rotation, glm::vec3 scale, glm::vec3 color)
		:GameObject(mesh, shader, posistion, rotation, scale, color) {
		this->camera = camera;
	}

	void Update(float dt) {


		UpdateModelMatrix();
	}

	void ProcessKeyboard(GLFWwindow* window, float dt) {
		bool Down, Held, UP;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			//std::cout << "UP" << std::endl;
			camera->Position.z -= MovementSpeed * dt;
			transform.posisiton.z -= MovementSpeed * dt;
		}
	}



	float MovementSpeed = 10;
	Camera* camera;

};
Player* player;

/*std::vector<Object> gameobjects{
	{glm::vec3(0,0,0),glm::vec3(0,0,0),glm::vec3(500, 0.2, 500),glm::vec3(1.0, 0.7, 0.6)},
	{glm::vec3(0,0,0),glm::vec3(0,0,0),glm::vec3(1,1,1),glm::vec3(1, 0.1, 0.1)},
};*/



float randRange(float min, float max) {
	int range = max - min + 1;
	float num = rand() % range + min;
	return num;
}

//PhysicsWorld pWorld;

GLFWwindow* window;


int Init() {

	//Initialize GLFW
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	//Create a window
	window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);
	if (window == NULL) {
		std::cout << "Failed to create a GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Initialize GLEW to get the newest opengl
	glewInit();

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	//Set the area were opengl will render too
	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glEnable(GL_DEPTH_TEST);

}

int main(void) {
	//Initialize GLFW and setup a window
	Init();
	
	//Shader lightingShader("res/shaders/Lit.vert", "res/shaders/Lit.frag");
	Shader lightCubeShader("res/shaders/light.vert", "res/shaders/light.frag");
	//Shader basicColor("res/shaders/colour.vert", "res/shaders/colours.frag");


	std::vector<Vertex> newVertices{
		//front
		{glm::vec3(-0.5,-0.5, 0.5),  glm::vec3(0,0,1.0), glm::vec2(0,0)},//0
		{glm::vec3(0.5,-0.5, 0.5),  glm::vec3(0,0,1.0), glm::vec2(0,0)},//1
		{glm::vec3(0.5, 0.5, 0.5),  glm::vec3(0,0,1.0), glm::vec2(0,0)},//2
		{glm::vec3(-0.5, 0.5, 0.5),  glm::vec3(0,0,1.0), glm::vec2(0,0)},//3

		//back															 
		{glm::vec3(-0.5,-0.5,-0.5),  glm::vec3(0,0,-1), glm::vec2(0,0)}, //4
		{glm::vec3(0.5,-0.5,-0.5),  glm::vec3(0,0,-1), glm::vec2(0,0)}, //5
		{glm::vec3(0.5, 0.5,-0.5),  glm::vec3(0,0,-1), glm::vec2(0,0)}, //6
		{glm::vec3(-0.5, 0.5,-0.5),  glm::vec3(0,0,-1), glm::vec2(0,0)}, //7

		//left
		{glm::vec3(-0.5,-0.5,-0.5),  glm::vec3(-1,0,0), glm::vec2(0,0)}, //8
		{glm::vec3(-0.5,-0.5, 0.5),  glm::vec3(-1,0,0), glm::vec2(0,0)}, //9
		{glm::vec3(-0.5, 0.5,-0.5),  glm::vec3(-1,0,0), glm::vec2(0,0)}, //10
		{glm::vec3(-0.5, 0.5, 0.5),  glm::vec3(-1,0,0), glm::vec2(0,0)}, //11

		//right
		{glm::vec3(0.5, 0.5,  0.5),  glm::vec3(1,0,0), glm::vec2(0,0)}, //12
		{glm::vec3(0.5, 0.5, -0.5),  glm::vec3(1,0,0), glm::vec2(0,0)}, //13
		{glm::vec3(0.5,-0.5, -0.5),  glm::vec3(1,0,0), glm::vec2(0,0)}, //14
		{glm::vec3(0.5,-0.5,  0.5),  glm::vec3(1,0,0), glm::vec2(0,0)}, //15

		//bottom
		{glm::vec3(-0.5, -0.5,  -0.5),  glm::vec3(0,-1.0,0), glm::vec2(0,0)}, //16
		{glm::vec3(0.5, -0.5,  0.5),  glm::vec3(0,-1.0,0), glm::vec2(0,0)}, //17
		{glm::vec3(0.5, -0.5,  -0.5),  glm::vec3(0,-1.0,0), glm::vec2(0,0)}, //18
		{glm::vec3(-0.5, -0.5,  0.5),  glm::vec3(0,-1.0,0), glm::vec2(0,0)}, //19

		//top
		{glm::vec3(-0.5,  0.5,  -0.5),  glm::vec3(0,1.0,0), glm::vec2(0,0)}, //20
		{glm::vec3(0.5,  0.5,  -0.5),  glm::vec3(0,1.0,0), glm::vec2(0,0)}, //21
		{glm::vec3(0.5,  0.5,   0.5),  glm::vec3(0,1.0,0), glm::vec2(0,0)}, //22
		{glm::vec3(-0.5,  0.5,   0.5),  glm::vec3(0,1.0,0), glm::vec2(0,0)}, //23
	};

	std::vector<unsigned int> indices{
		4,5,6,
		6,7,4,

		0,2,1,
		0,3,2,

		11,8,10,
		8, 11, 9,

		12,13,14,
		14,15,12,

		16,17,18,
		17,16,19,

		20,21,22,
		22,23,20,
	};

	//create a cube mesh
	Mesh quad(newVertices, indices);
	
	Transform tran(glm::vec3(0,5,0), glm::vec3(45,22,78), glm::vec3(1));
	GameObject cube(&quad, &lightCubeShader, tran, glm::vec3(1.0, 0.1, 0.2));
	

	GameObject plane(&quad, &lightCubeShader, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(500, 0.2, 500), glm::vec3(1.0, 0.7, 0.6));

	player = new Player(&quad, &lightCubeShader, &camera, glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), glm::vec3(3, 2, 4), glm::vec3(0.3, 0.7, 0.4));

	std::vector<GameObject> cubeCirlce;

	srand(time(0));
	for (int i = 0; i < 1000; i++) {
		GameObject G(&quad, &lightCubeShader, glm::vec3(randRange(-250, 250), randRange(-50, 50) + 50, randRange(-250, 250)),
			glm::vec3(randRange(0, 360), randRange(0, 360), randRange(0, 360)),
			glm::vec3(randRange(0.5, 10), randRange(0.5, 10), randRange(0.5, 10)),
			glm::vec3((randRange(0, 255) / 255), (randRange(0, 255) / 255), (randRange(0, 255) / 255)));
		cubeCirlce.push_back(G);
	}

	while (!glfwWindowShouldClose(window)) {
		//{ Timer timer;
		

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Inputs
		processInput(window);

		//Rendering
		glClearColor(0.9, 0.9, 1.0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		//lightingShader.setMat4("projection", projection);
		//lightingShader.setMat4("view", view);


		glm::mat4 model = glm::mat4(1.0f);

		lightCubeShader.use();
		lightCubeShader.setVec3("lightColor", glm::vec3(1.0));
		lightCubeShader.setVec3("lightPos", glm::vec3(1000, 800, 1000));

		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		
		//lightCubeShader.setVec3("objectColor", glm::vec3(1.0, 0.7, 0.6));
		//
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0, -0.5, 0));
		//model = glm::scale(model, glm::vec3(25, 0.1, 25));
		//lightCubeShader.setMat4("model", model);
		//quad.Draw();

		//timer += deltaTime;

		//if (timer >= 2) {
		//	gameobjects.push_back({ glm::vec3(x,5,0), glm::vec3(0,0,0), glm::vec3(0,0,0), 1 });
		//
		//	pWorld.AddObject(&gameobjects.back());
		//
		//	std::cout << gameobjects.size() << std::endl;
		//	timer = 0;
		//	x+=2;
		//}
		//std::cout << timer << std::endl;
		//pWorld.Step(deltaTime);

		/*for (Object obj : gameobjects) {
			lightCubeShader.use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, obj.pos);
			model = glm::rotate(model, glm::radians(obj.Rot.x), glm::vec3(1,0,0));
			model = glm::rotate(model, glm::radians(obj.Rot.y), glm::vec3(0,1,0));
			model = glm::rotate(model, glm::radians(obj.Rot.z), glm::vec3(0,0,1));
			model = glm::scale(model, obj.Scale);

			lightCubeShader.setMat4("model", model);
			lightCubeShader.setVec3("objectColor", obj.color);
			quad.Draw();
		}*/

		//lightCubeShader.use();
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(5,0,0));
		//model = glm::rotate(model, glm::radians(obj.Rot.x), glm::vec3(1, 0, 0));
		//model = glm::rotate(model, glm::radians(obj.Rot.y), glm::vec3(0, 1, 0));
		//model = glm::rotate(model, glm::radians(obj.Rot.z), glm::vec3(0, 0, 1));
		//model = glm::scale(model, glm::vec3(2));
		//
		//lightCubeShader.setMat4("model", model);
		//lightCubeShader.setVec3("objectColor", glm::vec3(1,0,1));
		//quad.Draw();

		//Update
		cube.Update(deltaTime);
		plane.Update(deltaTime);

		player->setPosistion(camera.Position + camera.Front);
		player->setScale(glm::vec3(0.1));
		player->setScale(glm::vec3(0.05,0.05,0.05));
		player->setRotation(glm::vec3(0, -camera.Yaw, camera.Pitch));
		player->Update(deltaTime);

		//Render
		cube.Render();
		plane.Render();
		
		player->Render();

		for (auto G : cubeCirlce) {
			G.Update(deltaTime);
			G.Render();
		}

		//----------------------------------
		//check & call events & swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		//}
	}

	glfwTerminate();
	return 0;
}


void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		glLineWidth(4);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		glPointSize(5);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		camera.ProcessKeyboard(FORWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		camera.ProcessKeyboard(BACKWORD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		camera.ProcessKeyboard(LEFT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		camera.ProcessKeyboard(RIGHT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
//		camera.ProcessKeyboard(UP, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
//		camera.ProcessKeyboard(DOWN, deltaTime);
	
	//player->ProcessKeyboard(window, deltaTime);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	camera.ProcessMouseScroll(yOffset);
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

unsigned int loadTexture(const char* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1) {
			format = GL_RED;
		} else if (nrComponents == 3) {
			format = GL_RGB;
		} else if (nrComponents == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	} else {
		std::cout << "FAILED TO LOAD TEXTURE AT PATH:" << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}