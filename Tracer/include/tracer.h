#pragma once
#include <algorithm>

#include <glm\glm.hpp>

#include "RawImage.h"
#include "geometry.h"


inline
float clamp(const float &lo, const float &hi, const float &v) {
	return std::max(lo, std::min(hi, v));
}
inline 
float deg2rad(const float &deg) {
	return deg * M_PI / 180;
}

struct RenderOptions
{
	uint32_t width;
	uint32_t height;
	float fov;
	glm::mat4x4 cameraToWorld;
};

bool trace(const glm::vec3 &orig, const glm::vec3 &dir, const std::vector<std::unique_ptr<Object>> &objects, float &tNear, const Object *&hitObject) {
	tNear = cInfinity;
	std::vector<std::unique_ptr<Object>>::const_iterator iter = objects.begin();
	for (; iter != objects.end(); ++iter) {
		float t = cInfinity;
		if ((*iter)->intersect(orig, dir, t) && t < tNear) {
			hitObject = iter->get();
			tNear = t;
		}
	}
	return (hitObject != nullptr);
}

glm::vec3 castRay(
	const glm::vec3 &orig, const glm::vec3 &dir,
	const std::vector<std::unique_ptr<Object>> &objects)
{
	glm::vec3 hitColor(1.0f, 0.0f, 0.0f);
	const Object *hitObject = nullptr;
	float t;
	if (trace(orig, dir, objects, t, hitObject)) {
		glm::vec3 Phit = orig + dir * t;
		glm::vec3 Nhit;
		glm::vec2 tex;
		hitObject->getSurfaceData(Phit, Nhit, tex);
		float scale = 4;
		float pattern = (fmodf((float)tex.x * scale, 1.0f) > 0.5f) ^ (fmodf((float)tex.y * scale, 1.0f) > 0.5f);
		hitColor = std::max(0.0f, glm::dot(Nhit, -dir)) * glm::mix(hitObject->color, hitObject->color * 0.8f, pattern);
	}

	return hitColor;
}


// RENDER FUNC
void render(const RenderOptions &options, std::vector<std::unique_ptr<Object>> &objects, RawImage &renderTarget) {
	float scale = tan(deg2rad(options.fov * 0.5f));
	float imageAspectRatio = options.width / (float)options.height;
	int pixIndex = 0;
	glm::vec3 orig = glm::vec3(options.cameraToWorld * glm::vec4(glm::vec3(0.0), 1.0));
	for (unsigned int j = 0; j < options.height; ++j) {
		for (unsigned int i = 0; i < options.width; ++i) {
			float x = (2 * (i + 0.5f) / (float)options.width - 1) * imageAspectRatio * scale;
			float y = (1 - 2 * (j + 0.5f) / (float)options.height) * scale;
			glm::vec3 dir = glm::vec3(options.cameraToWorld * glm::vec4(x, y, -1, 1.0f));
			glm::normalize(dir);
			renderTarget.setPixel(i, j, castRay(orig, dir, objects));
		}
	}
}