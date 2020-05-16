#include "SphericalCoordinate.h"

// https://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates

SphericalCoordinate::SphericalCoordinate(float theta, float phi, float rho) :
	theta(theta), phi(phi), rho(rho)
{
}

SphericalCoordinate::SphericalCoordinate(const glm::vec3& cartesian)
{
	rho = glm::length(cartesian);

	theta = acosf(cartesian.z / rho);
	phi = atanf(cartesian.y / cartesian.x);
}

SphericalCoordinate::~SphericalCoordinate()
{
}

SphericalCoordinate::operator glm::vec3() const
{
	return toCartesianCoordinate();
}

glm::vec3 SphericalCoordinate::toCartesianCoordinate() const
{
	float x = rho * sinf(theta) * cosf(phi);
	float y = rho * sinf(theta) * sinf(phi);
	float z = rho * cosf(theta);

	return glm::vec3(x, y, z);
}

SphericalCoordinate SphericalCoordinate::normalize() const
{
	return SphericalCoordinate(theta, phi);
}


