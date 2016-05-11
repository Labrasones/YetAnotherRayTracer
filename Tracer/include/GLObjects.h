#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include <glm\glm.hpp>
#include <glad\glad.h>

#include "RawImage.h"

struct GLVertex {
	glm::vec2 pos;
	glm::vec2 uv;
};
class GLMesh {
public:
	std::vector<GLVertex> verts;
	std::vector<GLuint> indicies;
	GLMesh(const std::vector<GLVertex> &vertices, const std::vector<GLuint> &indicies);
	~GLMesh();
	void draw();
private:
	GLuint vao, vbo, ebo;
	void _initGLMesh();
	void _destroyGLMesh();
};

class GLImage {
public:
	GLImage(unsigned int width, unsigned int height);
	~GLImage();
	// Very slow!
	void update();
	void bind();
	RawImage img;
private:
	GLuint id;
	void _init_glImage();
	void _destroy_glImage();
};

class GLShader {
public:
	GLShader(std::string shaderName);
	~GLShader();
	void bind();
private:
	GLuint programId, vertId, fragId;
	void _initGLShader(const GLchar * vertCode, const GLchar * fragCode);
	void _destroyGLShader();
	std::string readShaderFile(const std::string shaderPath);
};