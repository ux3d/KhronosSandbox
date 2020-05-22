#ifndef MATH_FRUSTUM_H_
#define MATH_FRUSTUM_H_

#include "glm_include.h"

#include "Plane.h"

class Sphere;

class Frustum {

private:

	Plane sidesWorld[6];

public:
	Frustum() = delete;
	Frustum(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	~Frustum();

	void updateViewProjection(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	bool isVisible(const glm::vec4& point) const;
	bool isVisible(const Sphere& sphere) const;

};

#endif /* MATH_FRUSTUM_H_ */
