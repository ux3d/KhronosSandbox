#ifndef MATH_SPHERICALCOORDINATE_H_
#define MATH_SPHERICALCOORDINATE_H_

#include "glm_include.h"

class SphericalCoordinate
{
private:
	float theta = 0.0f;
	float phi = 0.0f;
	float rho = 1.0f;
public:
	SphericalCoordinate(float theta, float phi, float rho = 1.0f);
	SphericalCoordinate(const glm::vec3& cartesian);
	~SphericalCoordinate();

	operator glm::vec3() const;
	glm::vec3 toCartesianCoordinate() const;

	SphericalCoordinate normalize() const;
};

#endif /* MATH_SPHERICALCOORDINATE_H_ */
