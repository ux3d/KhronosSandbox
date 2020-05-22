#ifndef MATH_SPHERE_H_
#define MATH_SPHERE_H_

#include "glm_include.h"

class Aabb;
class Plane;

class Sphere
{
private:

	glm::vec4 center = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float radius = 1.0f;

public:

	Sphere(const glm::vec4& center, float radius);

	~Sphere();

	const glm::vec4& getCenter() const;

	float getRadius() const;

	float distance(const glm::vec4& point) const;

	float distance(const Plane& plane) const;

	float distance(const Sphere& sphere) const;

	float distance(const Aabb& aabb) const;

	bool intersect(const glm::vec4& point) const;

	bool intersect(const Plane& plane) const;

	bool intersect(const Sphere& sphere) const;

	bool intersect(const Aabb& aabb) const;

	operator Aabb() const;
	Aabb toAabb() const;

	Sphere operator *(const glm::mat4& transform) const;

};

Sphere operator *(const glm::mat4& transform, const Sphere& sphere);

#endif /* MATH_SPHERE_H_ */
