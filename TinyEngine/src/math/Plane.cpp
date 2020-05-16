#include "Plane.h"

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

float Plane::distance(const glm::vec4& point) const
{
	return glm::dot(getNormal(), glm::vec3(point)) + getD();
}

Plane Plane::operator* (const glm::mat4& matrix) const
{
	glm::vec4 temp = glm::transpose(glm::inverse(matrix)) * glm::vec4(this->getNormal(), this->getD());

	return Plane(glm::vec3(temp), temp.w);
}
