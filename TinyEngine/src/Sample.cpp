#include "Sample.h"

// see https://cg.informatik.uni-freiburg.de/course_notes/graphics2_08_renderingEquation.pdf
// and https://www.tobias-franke.eu/log/2014/03/30/notes_on_importance_sampling.html
// and https://agraphicsguy.wordpress.com/2015/11/01/sampling-microfacet-brdf/

glm::vec3 Sample::uniformToCartesian(const glm::vec2& point)
{
	float cosTheta = 1.0 - point.x;

	float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
	float phi = point.y * 2.0f * glm::pi<float>();

	float x = sinTheta * sinf(phi);
	float y = cosTheta;
	float z = sinTheta * cosf(phi);

	return glm::vec3(x, y, z);
}

Spherical Sample::uniformToSpherical(const glm::vec2& point)
{
	float theta = acosf(1.0 - point.x);

	float phi = point.y * 2.0f * glm::pi<float>();

	return Spherical(theta, phi);
}

glm::vec3 Sample::lambertToCartesian(const glm::vec2& point)
{
	float sinTheta = sqrtf(point.x);

	float cosTheta = sqrtf(1.0f - sinTheta * sinTheta);
	float phi = point.y * 2.0f * glm::pi<float>();

	float x = sinTheta * sinf(phi);
	float y = cosTheta;
	float z = sinTheta * cosf(phi);

	return glm::vec3(x, y, z);
}

Spherical Sample::lambertToSpherical(const glm::vec2& point)
{
	float theta = asinf(sqrtf(point.x));

	float phi = point.y * 2.0f * glm::pi<float>();

	return Spherical(theta, phi);
}

glm::vec3 Sample::ggxToCartesian(const glm::vec2& point, float alpha)
{
	float cosTheta = sqrtf((1.0f - point.x) / (point.x * (alpha * alpha - 1.0f) + 1.0f));

	float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
	float phi = point.y * 2.0f * glm::pi<float>();

	float x = sinTheta * sinf(phi);
	float y = cosTheta;
	float z = sinTheta * cosf(phi);

	return glm::vec3(x, y, z);
}

Spherical Sample::ggxToSpherical(const glm::vec2& point, float alpha)
{
	float theta = acosf(sqrtf((1.0f - point.x) / (point.x * (alpha * alpha - 1.0f) + 1.0f)));

	float phi = point.y * 2.0f * glm::pi<float>();

	return Spherical(theta, phi);
}
