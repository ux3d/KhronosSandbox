#include "Sphere.h"

Sphere::Sphere(const glm::vec4& center, float radius) :
	center(center), radius(glm::abs(radius))
{
}

Sphere::~Sphere()
{
}

const glm::vec4& Sphere::getCenter() const
{
	return center;
}

float Sphere::getRadius() const
{
	return radius;
}

float Sphere::distance(const glm::vec4& point) const
{
	return glm::distance(center, point) - radius;
}

float Sphere::distance(const Plane& plane) const
{
	return plane.distance(*this);
}

float Sphere::distance(const Sphere& sphere) const
{
	return glm::distance(center, sphere.getCenter()) - radius - sphere.getRadius();
}

Sphere Sphere::operator *(const glm::mat4& transform) const
{
	float sx = glm::length(glm::vec3(transform[0]));
	float sy = glm::length(glm::vec3(transform[1]));
	float sz = glm::length(glm::vec3(transform[2]));

	if (sx != sy || sy != sz)
	{
		throw "transform not orthogonal";
	}

	return Sphere(transform * center, radius * sx);
}

Sphere operator *(const glm::mat4& transform, const Sphere& sphere)
{
	float sx = glm::length(glm::vec3(transform[0]));
	float sy = glm::length(glm::vec3(transform[1]));
	float sz = glm::length(glm::vec3(transform[2]));

	if (sx != sy || sy != sz)
	{
		throw "transform not orthogonal";
	}

	return Sphere(transform * sphere.getCenter(), sphere.getRadius() * sx);
}
