#ifndef MATH_PLANE_H_
#define MATH_PLANE_H_

#include "glm_include.h"

class Sphere;

class Plane
{
private:

	glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
	float d = 0.0f;

public:

	Plane();
	Plane(const glm::vec3& normal, float d);
	Plane(const glm::vec3& normal, const glm::vec4& point);

	~Plane();

	const glm::vec3& getNormal() const;

	float getD() const;

	float signedDistance(const glm::vec4& point) const;

	float distance(const glm::vec4& point) const;

	float distance(const Plane& plane) const;

	float distance(const Sphere& sphere) const;

	bool intersect(const glm::vec4& point) const;

	bool intersect(const Plane& plane) const;

	bool intersect(const Sphere& sphere) const;

	Plane operator* (const glm::mat4& matrix) const;

};

Plane operator* (const glm::mat4& matrix, const Plane& plane);

#endif /* MATH_PLANE_H_ */
