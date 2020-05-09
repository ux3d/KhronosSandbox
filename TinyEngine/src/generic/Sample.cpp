#include "Sample.h"

// see https://cg.informatik.uni-freiburg.de/course_notes/graphics2_08_renderingEquation.pdf
// and https://www.tobias-franke.eu/log/2014/03/30/notes_on_importance_sampling.html
// and https://agraphicsguy.wordpress.com/2015/11/01/sampling-microfacet-brdf/

glm::vec3 Sample::lambertToCartesian(const glm::vec2& point)
{
	float cosTheta = sqrtf(1.0f - point.x);

	float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
	float phi = point.y * 2.0f * glm::pi<float>();

	float x = sinTheta * cosf(phi);
	float y = sinTheta * sinf(phi);
	float z = cosTheta;

	return glm::vec3(x, y, z);
}

Spherical Sample::lambertToSpherical(const glm::vec2& point)
{
	float theta = acosf(sqrtf(1.0f - point.x));

	float phi = point.y * 2.0f * glm::pi<float>();

	return Spherical(theta, phi);
}

glm::vec3 Sample::ggxToCartesian(const glm::vec2& point, float alpha)
{
	float cosTheta = sqrtf((1.0f - point.x) / (point.x * (alpha * alpha - 1.0f) + 1.0f));

	float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
	float phi = point.y * 2.0f * glm::pi<float>();

	float x = sinTheta * cosf(phi);
	float y = sinTheta * sinf(phi);
	float z = cosTheta;

	return glm::vec3(x, y, z);
}

Spherical Sample::ggxToSpherical(const glm::vec2& point, float alpha)
{
	float theta = acosf(sqrtf((1.0f - point.x) / (point.x * (alpha * alpha - 1.0f) + 1.0f)));

	float phi = point.y * 2.0f * glm::pi<float>();

	return Spherical(theta, phi);
}
