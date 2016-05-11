/***
*     __  __    __    ___           __  __            ___              ______
*     \ \/ /__ / /_  / _ | ___ ___ / /_/ /____ ____  / _ \___  __ __  /_  __/___ __ ________ ____
*      \  / -_/ __/ / __ |/ _ / _ / __/ _ / -_/ __/ / , _/ _ `/ // /   / / / __/ _ `/ __/ -_/ __/
*      /_/\__/\__/ /_/ |_/_//_\___\__/_//_\__/_/   /_/|_|\_,_/\_, /   /_/ /_/  \_,_/\__/\__/_/
*                                                            /___/
*
*		Yes, it's nothing new and fancy. No, I don't intend it to be. However, even fundamentals 
*	can teach a great deal. 
*	With the guidence of Scratchapixel's in depth ray tracing tutorials (http://www.scratchapixel.com/index.php), 
*	this simple ray tracing application is an engaging and interesting way to build all kinds of skills and
*	experience with C++
*
*		Credit where credit is due. Because this project leans heavily on Scratchapixel's tutorials, much of
*	the codebase will closely replicate code provided by the author of Scratchapixel. However, this project
*	does not follow the structure of Scratchapixel. The focus of this project _is_ the modifications that
*	need to be made to allow for a different structure.
*/

#define NOMINMAX

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <cmath>
#include <thread>
#include <fstream>
#include <iostream>

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "specializedMath.h"
#include "GLObjects.h"
#include "tracer.h"
#include "geometry.h"

#define RENDER_RES_X 255
#define RENDER_RES_Y 255

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
static void window_size_callback(GLFWwindow* window, int width, int height) {
	double targetRatio = (double)RENDER_RES_Y / (double)RENDER_RES_X;
	double setRatio = (double)height / (double)width;
	if (abs(targetRatio - setRatio) > 0.01) { // Only adjust the ratio if it's not correct
		glfwSetWindowSize(window, width, (int)targetRatio*width);
	}
}
int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(RENDER_RES_X, RENDER_RES_Y, "YART - Render Window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	srand(time(NULL));

	std::vector<GLVertex> vertices;
	GLVertex v1;
	v1.pos = glm::vec2(-1.0f, 1.0f); 
	v1.uv = glm::vec2(0.0f, 0.0f);
	vertices.push_back(v1);
	GLVertex v2;
	v2.pos = glm::vec2(-1.0f, -1.0f);
	v2.uv = glm::vec2(0.0f, 1.0f);
	vertices.push_back(v2);
	GLVertex v3;
	v3.pos = glm::vec2(1.0f, -1.0f);
	v3.uv = glm::vec2(1.0f, 1.0f);
	vertices.push_back(v3);
	GLVertex v4;
	v4.pos = glm::vec2(1.0f, 1.0f);
	v4.uv = glm::vec2(1.0f, 0.0f);
	vertices.push_back(v4);

	std::vector<GLuint> elements = {
		0, 1, 2,
		2, 3, 0
	};

	GLMesh fullscreenQuad(vertices, elements);

	GLShader basicTextureShader("resource/basicTexture");
	basicTextureShader.bind();

	GLImage target(RENDER_RES_X, RENDER_RES_Y);
	//RawImage target(RENDER_RES_X, RENDER_RES_Y);
	int x = 0;
	int y = 0;
	RenderOptions options;
	options.width = RENDER_RES_X;
	options.height = RENDER_RES_Y;
	options.fov = 51.52f;
	options.cameraToWorld = glm::perspectiveFov(options.fov, (float)options.width, (float)options.height, 500.0f, 1.0f);

	std::vector<std::unique_ptr<Object>> objects;

	// generate a scene made of random spheres
	uint32_t numSpheres = 32;
	gen.seed(0);
	for (uint32_t i = 0; i < numSpheres; ++i) {
		glm::vec3 randPos((0.5f - dis(gen)) * 10, (0.5f - dis(gen)) * 10, (0.5f + dis(gen) * 10));
		float randRadius = (0.5f + dis(gen) * 0.5f);
		objects.push_back(std::unique_ptr<Object>(new Sphere(randPos, randRadius)));
	}

	std::thread renderThread(render, std::ref(options), std::ref(objects), std::ref(target.img)); // Start rendering on a seperate thread to allow the display to update throughout the render // TODO: losen syncronization between display and render

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		target.update(); // TODO: Losen syncronization for update. Might not need to lock the data being read from
		fullscreenQuad.draw();

		glfwSwapBuffers(window);
	}
	
	renderThread.join();

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
