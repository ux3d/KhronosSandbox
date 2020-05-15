#ifndef MATH_PROJECTION_H_
#define MATH_PROJECTION_H_

#include "Math.h"

class Projection
{
public:

	static glm::mat4 ortho(float left, float right, float bottom, float top, float nearVal, float farVal);
	static glm::mat4 perspective(float fovy, float aspect, float zNear, float zFar);
	static glm::mat4 frustum(float left, float right, float bottom, float top, float nearVal, float farVal);

};

#endif /* MATH_PROJECTION_H_ */
