#include "Sphere.h"

#include "Aabb.h"
#include "Plane.h"

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

float Sphere::distance(const Aabb& aabb) const
{
	return aabb.distance(*this);
}

bool Sphere::intersect(const glm::vec4& point) const
{
	return distance(point) <= 0.0f;
}

bool Sphere::intersect(const Plane& plane) const
{
	return distance(plane) <= 0.0f;
}

bool Sphere::intersect(const Sphere& sphere) const
{
	return distance(sphere) <= 0.0f;
}

bool Sphere::intersect(const Aabb& aabb) const
{
	return aabb.intersect(*this);
}

Sphere::operator Aabb() const
{
	return toAabb();
}

Aabb Sphere::toAabb() const
{
	glm::vec4 minimumCorner(center[0] - radius, center[1] - radius, center[2] - radius, 1.0f);
	glm::vec4 maximumCorner(center[0] + radius, center[1] + radius, center[2] + radius, 1.0f);

	return Aabb(minimumCorner, maximumCorner);
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
