#include "Sample.h"

#include <cmath>

glm::vec2 Sample::concentricDisk(const glm::vec2& point)
{
	glm::vec2 mapped = 2.0f * point - glm::vec2(1.0f, 1.0f);

	if (mapped.x == 0.0f && mapped.y == 0.0)
	{
		return glm::vec2(0.0f, 0.0f);
	}

	float theta;
	float r;

	if (fabsf(mapped.x) > fabsf(mapped.y))
	{
	   r = mapped.x;
	   theta = glm::pi<float>() / 4.0f * (mapped.y / mapped.x);
	}
	else
	{
	    r = mapped.y;
	    theta = glm::pi<float>() / 2.0f - glm::pi<float>() / 4.0f * (mapped.x / mapped.y);
	}

	return r * glm::vec2(cosf(theta), sinf(theta));
}

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
	glm::vec2 d = concentricDisk(point);

	float z = sqrtf(1.0f - d.x * d.x - d.y * d.y);

	return glm::vec3(d.x, d.y, z);
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
