#include "Frustum.h"

#include <cstdint>

#include "Sphere.h"

Frustum::Frustum(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) :
	sidesWorld()
{
	updateViewProjection(projectionMatrix, viewMatrix);
}

Frustum::~Frustum()
{
}

void Frustum::updateViewProjection(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	static const Plane sidesNDC[6] = {Plane(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f), Plane(glm::vec3(-1.0f, 0.0f, 0.0f), 1.0f), Plane(glm::vec3(0.0f, -1.0f, 0.0f), 1.0f), Plane(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f), Plane(glm::vec3(0.0f, 0.0f, 1.0f), 0.0f), Plane(glm::vec3(0.0f, 0.0f, -1.0f), 1.0f)};

	glm::mat4 transposedViewProjectionMatrix = glm::transpose(projectionMatrix * viewMatrix);

	for (uint32_t i = 0; i < 6; i++)
	{
		sidesWorld[i] = transposedViewProjectionMatrix * sidesNDC[i];
	}
}

bool Frustum::isVisible(const glm::vec4& point) const
{
	for (const auto& currentSide : sidesWorld)
	{
		if (currentSide.signedDistance(point) < 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool Frustum::isVisible(const Sphere& sphere) const
{
	for (const auto& currentSide : sidesWorld)
	{
		float signedDistance = currentSide.signedDistance(sphere.getCenter());

		if (signedDistance + sphere.getRadius() < 0.0f)
		{
			return true;
		}
	}

	return false;
}

