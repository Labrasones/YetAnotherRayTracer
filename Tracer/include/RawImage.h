#pragma once
#include <mutex>

#include <glm/glm.hpp>

class RawImage {
public:

	RawImage(unsigned int width, unsigned int height) : width(width), height(height) {
		_data = new glm::vec3[width * height];
		for (unsigned int i = 0; i < width * height; i++) {
			_data[i].r = 0.0f;
			_data[i].g = 0.0f;
			_data[i].b = 0.0f;
		}
	}
	~RawImage() {
		delete[] _data;
	}

	void setPixel(int x, int y, float r, float g, float b) {
		threadLock.lock(); // Lock for edit
		_data[x + width*y].r = r;
		_data[x + width*y].g = g;
		_data[x + width*y].b = b;
		threadLock.unlock(); // Done edit
	}
	void setPixel(int x, int y, const glm::vec3 &rgb) {
		setPixel(x, y, rgb.r, rgb.g, rgb.b);
	}
	void setPixel(const glm::vec2 &xy, const glm::vec3 &rgb) {
		setPixel((int)xy.x, (int)xy.y, rgb);
	}
	glm::vec3 * getDataPtr() {
		return _data;
	}
	unsigned int width;
	unsigned int height;
	std::mutex threadLock;

private:
	glm::vec3 * _data;
};