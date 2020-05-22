#ifndef MATH_SAMPLE_H_
#define MATH_SAMPLE_H_

#include "glm_include.h"

#include "SphericalCoordinate.h"

class Sample {
public:

	static glm::vec3 lambertToCartesianCoordinate(const glm::vec2& point);

	static SphericalCoordinate lambertToSphericalCoordinate(const glm::vec2& point);

	static glm::vec3 ggxToCartesianCoordinate(const glm::vec2& point, float alpha);

	static SphericalCoordinate ggxToSphericalCoordinate(const glm::vec2& point, float alpha);

};

#endif /* MATH_SAMPLE_H_ */
