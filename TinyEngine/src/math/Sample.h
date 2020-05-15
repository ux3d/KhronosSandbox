#ifndef MATH_SAMPLE_H_
#define MATH_SAMPLE_H_

#include "Math.h"

// https://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates
class Spherical {
public:
	float theta = 0.0f;
	float phi = 0.0f;
	float rho = 1.0f;

	Spherical(float theta, float phi, float rho = 1.0f) :
		theta(theta), phi(phi), rho(rho)
	{
	}

	Spherical(const glm::vec3& cartesian)
	{
		rho = glm::length(cartesian);

		theta = acosf(cartesian.z / rho);
		phi = atanf(cartesian.y / cartesian.x);
	}

	glm::vec3 toCartesian() const
	{
		float x = rho * sinf(theta) * cosf(phi);
		float y = rho * sinf(theta) * sinf(phi);
		float z = rho * cosf(theta);

		return glm::vec3(x, y, z);
	}

	Spherical normalize() const
	{
		return Spherical(theta, phi);
	}
};

class Sample {
public:

	static glm::vec3 lambertToCartesian(const glm::vec2& point);

	static Spherical lambertToSpherical(const glm::vec2& point);

	static glm::vec3 ggxToCartesian(const glm::vec2& point, float alpha);

	static Spherical ggxToSpherical(const glm::vec2& point, float alpha);

};

#endif /* MATH_SAMPLE_H_ */
