#ifndef MATH_AABB_H_
#define MATH_AABB_H_

#include "Math.h"

class Sphere;

class Aabb {
private:

	glm::vec4 minimumCorner = glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	glm::vec4 maximumCorner = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	glm::vec4 corners[8];

	void calculateCorners();

public:

	Aabb();
	Aabb(const glm::vec4& minimumCorner, const glm::vec4& maximumCorner);
	~Aabb();

	const glm::vec4& getMinimumCorner() const;
	const glm::vec4& getMaximumCorner() const;

	const glm::vec4& getCorner(uint32_t i) const;

	float distance(const glm::vec4& point) const;

	float distance(const Sphere& sphere) const;

	bool intersect(const glm::vec4& point) const;

	bool intersect(const Aabb& aabb) const;

	bool intersect(const Sphere& sphere) const;

	operator Sphere() const;
	Sphere toSphere() const;

	Aabb operator *(const glm::mat4& transform) const;

};

Aabb operator *(const glm::mat4& transform, const Aabb& aabb);

#endif /* MATH_AABB_H_ */
