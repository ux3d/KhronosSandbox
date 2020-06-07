#ifndef ACTIVITY_INTERPOLATOR_H_
#define ACTIVITY_INTERPOLATOR_H_

#include "../math/Math.h"

class Interpolator {
public:

	static float linear(float x, float y, float t);
	static glm::vec2 linear(const glm::vec2& x, const glm::vec2& y, float t);
	static glm::vec3 linear(const glm::vec3& x, const glm::vec3& y, float t);
	static glm::quat linear(const glm::quat& x, const glm::quat& y, float t);

	static float step(float x, float y, float t);
	static glm::vec2 step(const glm::vec2& x, const glm::vec2& y, float t);
	static glm::vec3 step(const glm::vec3& x, const glm::vec3& y, float t);
	static glm::quat step(const glm::quat& x, const glm::quat& y, float t);

	static float cubicspline(float x, float xOut, float yIn, float y, float t);
	static glm::vec2 cubicspline(const glm::vec2& x, const glm::vec2& xOut, const glm::vec2& yIn, const glm::vec2& y, float t);
	static glm::vec3 cubicspline(const glm::vec3& x, const glm::vec3& xOut, const glm::vec3& yIn, const glm::vec3& y, float t);
	static glm::quat cubicspline(const glm::quat& x, const glm::quat& xOut, const glm::quat& yIn, const glm::quat& y, float t);

};

#endif /* MATH_INTERPOLATOR_H_ */
