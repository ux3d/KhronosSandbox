#ifndef MATH_INTERPOLATOR_H_
#define MATH_INTERPOLATOR_H_

#include "glm_include.h"

class Interpolator {
public:

	static float linear(float x, float y, float t);
	static glm::vec3 linear(const glm::vec3& x, glm::vec3& y, float t);
	static glm::quat linear(const glm::quat& x, glm::quat& y, float t);

	static float step(float x, float y, float t);
	static glm::vec3 step(const glm::vec3& x, glm::vec3& y, float t);
	static glm::quat step(const glm::quat& x, glm::quat& y, float t);

	static float cubicspline(float x, float xOut, float yIn, float y, float t);
	static glm::vec3 cubicspline(const glm::vec3& x, const glm::vec3& xOut, const glm::vec3& yIn, glm::vec3& y, float t);
	static glm::quat cubicspline(const glm::quat& x, const glm::quat& xOut, const glm::quat& yIn, glm::quat& y, float t);

};

#endif /* MATH_INTERPOLATOR_H_ */
