#include "Sample.h"

#include <cmath>

glm::vec3 Sample::uniformHemisphere(const glm::vec2& point)
{
	float cosTheta = sqrtf(1.0f - point.x);

	float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
	float phi = point.y * 2.0f * glm::pi<float>();

	float x = sinTheta * cosf(phi);
	float y = sinTheta * sinf(phi);
	float z = cosTheta;

	return glm::vec3(x, y, z);
}

glm::vec3 Sample::cosineHemisphere(const glm::vec2& point)
{
	float cosTheta = sqrtf(1.0f - point.x);

	// Radius
	float sinTheta = sqrtf(point.x);
	float phi = point.y * 2.0f * glm::pi<float>();

	float x = sinTheta * cosf(phi);
	float y = sinTheta * sinf(phi);
	float z = cosTheta;

	return glm::vec3(x, y, z);
}

glm::vec3 Sample::ggxHemisphere(const glm::vec2& point, float alpha)
{
	float cosTheta = sqrtf((1.0f - point.x) / (point.x * (alpha * alpha - 1.0f) + 1.0f));

	float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
	float phi = point.y * 2.0f * glm::pi<float>();

	float x = sinTheta * cosf(phi);
	float y = sinTheta * sinf(phi);
	float z = cosTheta;

	return glm::vec3(x, y, z);
}
