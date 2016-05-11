#include "GLObjects.h"

GLMesh::GLMesh(const std::vector<GLVertex> &verticies, const std::vector<GLuint> &indicies) : verts(verticies), indicies(indicies) {
	_initGLMesh();
}
GLMesh::~GLMesh() {
	_destroyGLMesh();
}
void GLMesh::draw() {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void GLMesh::_initGLMesh() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLVertex), &verts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLuint), &indicies[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (GLvoid*)0);
	// Vertex Colours
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (GLvoid*)offsetof(GLVertex, uv));
	glBindVertexArray(0);
}
void GLMesh::_destroyGLMesh() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

GLImage::GLImage(unsigned int width, unsigned int height) : img(width, height) {
	_init_glImage();
}
GLImage::~GLImage() {
	_destroy_glImage();
}
void GLImage::bind() {
	glBindTexture(GL_TEXTURE_2D, id);
}
void GLImage::update() {
	bind();
	img.threadLock.lock(); // Lock for update
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.width, img.height, GL_RGB, GL_FLOAT, img.getDataPtr());
	img.threadLock.unlock(); // Updated OpenGL's data. Free edits again
}
void GLImage::_init_glImage() {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height, 0, GL_RGB, GL_FLOAT, img.getDataPtr());
}
void GLImage::_destroy_glImage() {
	glDeleteTextures(1, &id);
}

GLShader::GLShader(std::string shaderName) {
	std::string vertCode = readShaderFile(shaderName + ".vert");
	std::string fragCode = readShaderFile(shaderName + ".frag");
	_initGLShader(vertCode.c_str(), fragCode.c_str());
}
GLShader::~GLShader() {
	_destroyGLShader();
}
void GLShader::bind() {
	glUseProgram(programId);
}
std::string GLShader::readShaderFile(const std::string shaderPath) {
	std::ifstream shaderSource(shaderPath);
	if (!shaderSource) {
		std::cerr << "Error opening shader source at: " << shaderPath.c_str() << std::endl;
		return std::string();
	}
	std::string outputSource;
	shaderSource.seekg(0, std::ios::end);
	outputSource.resize(shaderSource.tellg());
	shaderSource.seekg(0, std::ios::beg);
	shaderSource.read(&outputSource[0], outputSource.size());
	shaderSource.close();
	return outputSource;
}
void GLShader::_initGLShader(const GLchar * vertCode, const GLchar * fragCode) {
	vertId = glCreateShader(GL_VERTEX_SHADER);
	fragId = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertId, 1, &vertCode, NULL);
	glCompileShader(vertId); // Need to check for errors

	glShaderSource(fragId, 1, &fragCode, NULL);
	glCompileShader(fragId); // Needs error check

	programId = glCreateProgram();
	glAttachShader(programId, vertId);
	glAttachShader(programId, fragId);
	glBindFragDataLocation(programId, 0, "outColor");
	glLinkProgram(programId);
	glDetachShader(programId, vertId);
	glDetachShader(programId, fragId);
	bind();
}
void GLShader::_destroyGLShader() {
	glDeleteShader(vertId);
	glDeleteShader(fragId);
	glUseProgram(0);
	glDeleteProgram(programId);
}
