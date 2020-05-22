#ifndef MATH_PROJECTION_H_
#define MATH_PROJECTION_H_

#include "glm_include.h"

class Projection
{
public:

	static glm::mat4 ortho(float left, float right, float bottom, float top, float nearVal, float farVal);
	static glm::mat4 perspective(float fovy, float aspect, float zNear, float zFar);
	static glm::mat4 frustum(float left, float right, float bottom, float top, float nearVal, float farVal);

	static glm::mat4 viewport(float x, float y, float width, float height, float nearVal, float farVal);
};

#endif /* MATH_PROJECTION_H_ */
