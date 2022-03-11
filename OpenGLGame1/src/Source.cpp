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
#include "PhysicsWorld.h"

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")


void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* widow, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char *path);

int WIDTH = 1280, HEIGHT = 720;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(5.0f, 5.0f, 25.0f));
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

std::vector<Object> gameobjects{
	{glm::vec3(-6,11,0), glm::vec3(2,6,1), glm::vec3(0,0,0), 1},
	{glm::vec3(-3,8,7), glm::vec3(0,0,0), glm::vec3(3,20,0), 10},
	{glm::vec3(6,11,0), glm::vec3(0,0,0), glm::vec3(5,5,5), 2},
	{glm::vec3(3,5,0), glm::vec3(0,0,0), glm::vec3(-5,5,5), 1},
	{glm::vec3(6,7,4), glm::vec3(0,0,0), glm::vec3(5,50,5), 1},
	{glm::vec3(0,7,0), glm::vec3(0,0,0), glm::vec3(30,100,0), 1},
};

PhysicsWorld pWorld;


int main(void) {

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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);
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

	//Shader lightingShader("res/shaders/Lit.vert", "res/shaders/Lit.frag");
	Shader lightCubeShader("res/shaders/light.vert", "res/shaders/light.frag");
	Shader basicColor("res/shaders/colour.vert", "res/shaders/colours.frag");


	std::vector<Vertex> newVertices{
		//front
		{glm::vec3(-0.5,-0.5, 0.5),  glm::vec3(0,0,1.0), glm::vec2(0,0)},//0
		{glm::vec3( 0.5,-0.5, 0.5),  glm::vec3(0,0,1.0), glm::vec2(0,0)},//1
		{glm::vec3( 0.5, 0.5, 0.5),  glm::vec3(0,0,1.0), glm::vec2(0,0)},//2
		{glm::vec3(-0.5, 0.5, 0.5),  glm::vec3(0,0,1.0), glm::vec2(0,0)},//3
																		 
		//back															 
		{glm::vec3(-0.5,-0.5,-0.5),  glm::vec3(0,0,-1), glm::vec2(0,0)}, //4
		{glm::vec3( 0.5,-0.5,-0.5),  glm::vec3(0,0,-1), glm::vec2(0,0)}, //5
		{glm::vec3( 0.5, 0.5,-0.5),  glm::vec3(0,0,-1), glm::vec2(0,0)}, //6
		{glm::vec3(-0.5, 0.5,-0.5),  glm::vec3(0,0,-1), glm::vec2(0,0)}, //7

		//left
		{glm::vec3(-0.5,-0.5,-0.5),  glm::vec3(-1,0,0), glm::vec2(0,0)}, //8
		{glm::vec3(-0.5,-0.5, 0.5),  glm::vec3(-1,0,0), glm::vec2(0,0)}, //9
		{glm::vec3(-0.5, 0.5,-0.5),  glm::vec3(-1,0,0), glm::vec2(0,0)}, //10
		{glm::vec3(-0.5, 0.5, 0.5),  glm::vec3(-1,0,0), glm::vec2(0,0)}, //11

		//right
		{glm::vec3( 0.5, 0.5,  0.5),  glm::vec3(1,0,0), glm::vec2(0,0)}, //12
		{glm::vec3( 0.5, 0.5, -0.5),  glm::vec3(1,0,0), glm::vec2(0,0)}, //13
		{glm::vec3( 0.5,-0.5, -0.5),  glm::vec3(1,0,0), glm::vec2(0,0)}, //14
		{glm::vec3( 0.5,-0.5,  0.5),  glm::vec3(1,0,0), glm::vec2(0,0)}, //15

		//bottom
		{glm::vec3(-0.5, -0.5,  -0.5),  glm::vec3(0,-1.0,0), glm::vec2(0,0)}, //16
		{glm::vec3(0.5, -0.5,  0.5),  glm::vec3(0,-1.0,0), glm::vec2(0,0)}, //17
		{glm::vec3(0.5, -0.5,  -0.5),  glm::vec3(0,-1.0,0), glm::vec2(0,0)}, //18
		{glm::vec3(-0.5, -0.5,  0.5),  glm::vec3(0,-1.0,0), glm::vec2(0,0)}, //19

		//top
		{glm::vec3(-0.5,  0.5,  -0.5),  glm::vec3(0,1.0,0), glm::vec2(0,0)}, //20
		{glm::vec3( 0.5,  0.5,  -0.5),  glm::vec3(0,1.0,0), glm::vec2(0,0)}, //21
		{glm::vec3( 0.5,  0.5,   0.5),  glm::vec3(0,1.0,0), glm::vec2(0,0)}, //22
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

	/*
	float vertices[] = {
		// positions          // normals           // texture coords
		//back face
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		//front face
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,

		//left face
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		
		//right face
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		//bottom
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,

		//top
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};


	//Create and link the VBO
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//UV attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	*/
	//unsigned int diffuseMap = loadTexture("res/textures/Planks014_2K-JPG/Planks014_2K_Color.jpg");
	//unsigned int specularMap = loadTexture("res/textures/Planks014_2K-JPG/Planks014_2K_Roughness.jpg");
	//unsigned int diffuseMap2 = loadTexture("res/textures/container2.png");
	//unsigned int specularMap2 = loadTexture("res/textures/container2_specular.png");

	glEnable(GL_DEPTH_TEST);

	//lightingShader.use();
	//lightingShader.setInt("material.diffuse", 0);
	//lightingShader.setInt("material.specular", 1);

	//create a cube mesh
	Mesh quad(newVertices, indices);

	Object cubeOBJ{glm::vec3(-6,11,0), glm::vec3(0), glm::vec3(0), 1};
	Object cubeOBJ1{glm::vec3(-3,8,7), glm::vec3(0), glm::vec3(0), 1};
	Object cubeOBJ2{glm::vec3(6,11,0), glm::vec3(0), glm::vec3(0), 1};

	//add gameobjects to the physics world
	//pWorld.AddObject(&gameobjects[0]);
	//pWorld.AddObject(&gameobjects[1]);
	//pWorld.AddObject(&gameobjects[2]);

	for (int i = 0; i < gameobjects.size(); i++) {
		pWorld.AddObject(&gameobjects[i]);
	}

	while (!glfwWindowShouldClose(window)) {
		{ Timer timer;

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Inputs
		processInput(window);

		//Rendering
		glClearColor(0.9, 0.9, 1.0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//lightPos.x = cos(glfwGetTime()) * 4;
		//lightPos.z = sin(glfwGetTime()) * 4;

		//lightingShader.use();
		//lightingShader.setVec3("viewPos", camera.Position);
		//lightingShader.setFloat("material.shininess", 32.0f);
		//lightingShader.setFloat("material.multiplier", 4.0f);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		//lightingShader.setMat4("projection", projection);
		//lightingShader.setMat4("view", view);


		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0));
		//model = glm::scale(model, glm::vec3(10.0f, 0.1f, 10.0f));
		//lightingShader.setMat4("model", model);

		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//lightingShader.setFloat("material.multiplier", 1.0f);ss

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.6f, 0));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		lightingShader.setMat4("model", model);
		*/

		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap2);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		*/

		lightCubeShader.use();
		lightCubeShader.setVec3("objectColor", glm::vec3(0.5, 1, 0.7));
		lightCubeShader.setVec3("lightColor", glm::vec3(1.0));
		lightCubeShader.setVec3("lightPos", glm::vec3(100, 80, 100));

		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		
		lightCubeShader.setVec3("objectColor", glm::vec3(1.0, 0.7, 0.6));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0, -0.5, 0));
		model = glm::scale(model, glm::vec3(25, 0.1, 25));
		lightCubeShader.setMat4("model", model);
		quad.Draw();
		/*
		lightCubeShader.setVec3("objectColor", glm::vec3(0.5, 1, 0.7));

		for (int x = 0; x < 5; x++) {
			for (int y = 0; y < 5; y++) {
				for (int z = 0; z < 5; z++) {
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(x * 1.25, y * 1.25, z * 1.25));
					model = glm::scale(model, glm::vec3(1));
					lightCubeShader.setMat4("model", model);
					quad.Draw();
				}
			}
		}

		lightCubeShader.setVec3("objectColor", glm::vec3(1));
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0,10,-2));
		//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(0.5));
		lightCubeShader.setMat4("model", model);
		quad.Draw();
		*/
		//--------------------------------

		/*cubeOBJ.pos += cubeOBJ.vel * deltaTime;
		if (cubeOBJ.pos.y < 0.1 + 1 - 0.5) {
			cubeOBJ.pos -= cubeOBJ.vel * deltaTime;
		}
		*/
		lightCubeShader.use();
		lightCubeShader.setVec3("objectColor", glm::vec3(1,0.1,0.1));

		

		pWorld.Step(deltaTime);

		for (Object obj : gameobjects) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, obj.pos);
			model = glm::scale(model, glm::vec3(2));
			lightCubeShader.setMat4("model", model);
			quad.Draw();
		}

		//----------------------------------
		//check & call events & swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		}
	}



	//glDeleteVertexArrays(1, &cubeVAO);
	//glDeleteVertexArrays(1, &lightCubeVAO);
	//glDeleteBuffers(1, &VBO);

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

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWORD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
		
	}
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