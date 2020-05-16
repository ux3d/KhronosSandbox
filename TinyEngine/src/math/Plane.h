#ifndef MATH_PLANE_H_
#define MATH_PLANE_H_

#include "Math.h"

class Plane
{
private:

	glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
	float d = 0.0f;

public:

	Plane(const glm::vec3& normal, float d);
	Plane(const glm::vec3& normal, const glm::vec4& point);

	~Plane();

	const glm::vec3& getNormal() const;

	float getD() const;

	float distance(const glm::vec4& point) const;

	Plane operator* (const glm::mat4& matrix) const;

};

#endif /* MATH_PLANE_H_ */
