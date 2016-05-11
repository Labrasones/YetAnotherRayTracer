#pragma once

#include <glm\glm.hpp>

#include "specializedMath.h"

class Object
{
public:
	Object() : color(dis(gen), dis(gen), dis(gen)) {}
	virtual ~Object() {}

	virtual bool intersect(const glm::vec3 &, const glm::vec3 &, float &) const = 0;
	virtual void getSurfaceData(const glm::vec3 &, glm::vec3 &, glm::vec2 &) const = 0;
	glm::vec3 color;
};

class Sphere : public Object
{
public:
	Sphere(const glm::vec3 &c, const float &r) : radius(r), radius2(r * r), centre(c) {}
	bool intersect(const glm::vec3 &orig, const glm::vec3 &dir, float &t) const
	{
		float t0, t1; // Intersect solutions
		glm::vec3 L = orig - centre;
		float a = glm::dot(dir, dir);// dir . dir == ||dir||^2
		float b = 2 * glm::dot(dir, L);
		float c = glm::dot(L, L) - radius2;
		if (!solveQuadratic(a, b, c, t0, t1)) {
			return false;
		}
		if (t0 > t1) {
			std::swap(t0, t1);
		}
		if (t0 < 0) {
			t0 = t1;
			if (t0 < 0) {
				return false;
			}
		}
		t = t0;
		return true;
	}
	void getSurfaceData(const glm::vec3 &Phit, glm::vec3 &Nhit, glm::vec2 &tex) const
	{
		Nhit = Phit - centre;
		glm::normalize(Nhit);
		tex.x = (1.0f + atan2(Nhit.z, Nhit.x) / M_PI) * 0.5f;
		tex.y = acosf(Nhit.y) / M_PI;
	}
	float radius, radius2;
	glm::vec3 centre;
};