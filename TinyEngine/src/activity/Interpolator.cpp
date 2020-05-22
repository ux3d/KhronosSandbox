#include "Interpolator.h"

#include <cstdint>

float Interpolator::linear(float x, float y, float t)
{
	return glm::mix(x, y, t);
}

glm::vec3 Interpolator::linear(const glm::vec3& x, glm::vec3& y, float t)
{
	return glm::mix(x, y, t);
}

glm::quat Interpolator::linear(const glm::quat& x, glm::quat& y, float t)
{
	return glm::slerp(x, y, t);
}

float Interpolator::step(float x, float y, float t)
{
	return (t == 1.0f) ? y : x;
}

glm::vec3 Interpolator::step(const glm::vec3& x, glm::vec3& y, float t)
{
	return (t == 1.0f) ? y : x;
}

glm::quat Interpolator::step(const glm::quat& x, glm::quat& y, float t)
{
	return (t == 1.0f) ? y : x;
}

float Interpolator::cubicspline(float x, float xOut, float yIn, float y, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
    float h10 = t3 - 2.0f * t2 + t;
    float h01 = -2.0f * t3 + 3.0f * t2;
    float h11 = t3 - t2;

    return h00 * x + h10 * xOut + h01 * y + h11 * yIn;
}

glm::vec3 Interpolator::cubicspline(const glm::vec3& x, const glm::vec3& xOut, const glm::vec3& yIn, glm::vec3& y, float t)
{
	glm::vec3 result;

	for (uint32_t i = 0; i < 3; i++)
	{
		result[i] = cubicspline(x[i], xOut[i], yIn[i], y[i], t);
	}

	return result;
}

glm::quat Interpolator::cubicspline(const glm::quat& x, const glm::quat& xOut, const glm::quat& yIn, glm::quat& y, float t)
{
	glm::quat result;

	for (uint32_t i = 0; i < 4; i++)
	{
		result[i] = cubicspline(x[i], xOut[i], yIn[i], y[i], t);
	}

	return glm::normalize(result);
}
