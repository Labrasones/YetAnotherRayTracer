#pragma once
#include <random>

#include <glm\glm.hpp>

#define M_PI 3.141592653589793f

const float cInfinity = std::numeric_limits<float>::max();
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0, 1);

bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0) {
		return false;
	}
	else {
		float q = (b > 0.0f) ? -0.5f * (b + std::sqrt(discr)) : -0.5f * (b - sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}
	return true;
}