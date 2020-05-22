#include "Plane.h"

#include "Sphere.h"

Plane::Plane() :
	normal(0.0f, 1.0f, 0.0f), d(0.0f)
{
}

Plane::Plane(const glm::vec3& normal, const float d) :
	normal(glm::normalize(normal)), d(d/glm::length(normal))
{
}

Plane::Plane(const glm::vec3& normal, const glm::vec4& point) :
	normal(glm::normalize(normal)), d(-glm::dot(glm::normalize(normal), glm::vec3(point)))
{
}

Plane::~Plane()
{
}

const glm::vec3& Plane::getNormal() const
{
	return normal;
}

float Plane::getD() const
{
	return d;
}

float Plane::signedDistance(const glm::vec4& point) const
{
	return glm::dot(getNormal(), glm::vec3(point)) + getD();
}

float Plane::distance(const glm::vec4& point) const
{
	return glm::abs(signedDistance(point));
}

float Plane::distance(const Plane& plane) const
{
	float NoN = glm::dot(normal, plane.getNormal());

	if (glm::abs(NoN) < 1.0f)
	{
		return 0.0f;
	}

	return glm::abs(getD() - glm::sign(NoN) * plane.getD());
}

float Plane::distance(const Sphere& sphere) const
{
	return distance(sphere.getCenter()) - sphere.getRadius();
}

bool Plane::intersect(const glm::vec4& point) const
{
	return distance(point) == 0.0f;
}

bool Plane::intersect(const Plane& plane) const
{
	return distance(plane) == 0.0f;
}

bool Plane::intersect(const Sphere& sphere) const
{
	return distance(sphere) <= 0.0f;
}

Plane Plane::operator* (const glm::mat4& matrix) const
{
	glm::vec4 temp = glm::transpose(glm::inverse(matrix)) * glm::vec4(this->getNormal(), this->getD());

	return Plane(glm::vec3(temp), temp.w);
}

Plane operator* (const glm::mat4& matrix, const Plane& plane)
{
	glm::vec4 temp = glm::transpose(glm::inverse(matrix)) * glm::vec4(plane.getNormal(), plane.getD());

	return Plane(glm::vec3(temp), temp.w);
}
