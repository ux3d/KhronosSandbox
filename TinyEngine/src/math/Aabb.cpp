#include "Aabb.h"

#include "Sphere.h"

void Aabb::calculateCorners()
{
	corners[0].x = minimumCorner.x;
	corners[1].x = minimumCorner.x;
	corners[2].x = minimumCorner.x;
	corners[3].x = minimumCorner.x;
	corners[4].x = maximumCorner.x;
	corners[5].x = maximumCorner.x;
	corners[6].x = maximumCorner.x;
	corners[7].x = maximumCorner.x;

	corners[0].y = minimumCorner.y;
	corners[1].y = minimumCorner.y;
	corners[2].y = maximumCorner.y;
	corners[3].y = maximumCorner.y;
	corners[4].y = minimumCorner.y;
	corners[5].y = minimumCorner.y;
	corners[6].y = maximumCorner.y;
	corners[7].y = maximumCorner.y;

	corners[0].z = minimumCorner.z;
	corners[1].z = maximumCorner.z;
	corners[2].z = minimumCorner.z;
	corners[3].z = maximumCorner.z;
	corners[4].z = minimumCorner.z;
	corners[5].z = maximumCorner.z;
	corners[6].z = minimumCorner.z;
	corners[7].z = maximumCorner.z;
}

Aabb::Aabb()
{
	calculateCorners();
}

Aabb::Aabb(const glm::vec4& minimumCorner, const glm::vec4& maximumCorner) :
	minimumCorner(minimumCorner), maximumCorner(maximumCorner)
{
	calculateCorners();
}

Aabb::~Aabb()
{
}

const glm::vec4& Aabb::getMaximumCorner() const
{
	return maximumCorner;
}

const glm::vec4& Aabb::getMinimumCorner() const
{
	return minimumCorner;
}

const glm::vec4& Aabb::getCorner(uint32_t i) const
{
	if (i >= 8)
	{
		throw "corner index out of bounds";
	}

	return corners[i];
}

float Aabb::distance(const glm::vec4& point) const
{
	glm::vec4 closestPoint(0.0f, 0.0f, 0.0f, 1.0f);

	for (uint32_t i = 0; i < 3; i++)
	{
		float v = point[i];
		if (v < minimumCorner[i])
		{
			v = minimumCorner[i];
		}
		if (v > maximumCorner[i])
		{
			v = maximumCorner[i];
		}
		closestPoint[i] = v;
	}

	return glm::distance(closestPoint, point);
}

float Aabb::distance(const Sphere& sphere) const
{
	return distance(sphere.getCenter()) - sphere.getRadius();
}

bool Aabb::intersect(const glm::vec4& point) const
{
	for (uint32_t i = 0; i < 3; i++)
	{
		if (point[i] < minimumCorner[i] || point[i] > maximumCorner[i])
		{
			return false;
		}
	}

	return true;
}

bool Aabb::intersect(const Aabb& aabb) const
{
	for (uint32_t i = 0; i < 3; i++)
	{
		if (maximumCorner[i] < aabb.minimumCorner[i] || aabb.maximumCorner[i] < minimumCorner[i])
		{
			return false;
		}
	}

	return true;
}

bool Aabb::intersect(const Sphere& sphere) const
{
	return distance(sphere) <= 0.0f;
}

Aabb::operator Sphere() const
{
	return toSphere();
}

Sphere Aabb::toSphere() const
{
	return Sphere((minimumCorner + maximumCorner) * 0.5f, glm::distance(minimumCorner, maximumCorner) * 0.5);
}

Aabb Aabb::operator *(const glm::mat4& transform) const
{
	glm::vec4 transformedCorners[8];
	for (uint32_t i = 0; i < 8; i++)
	{
		transformedCorners[i] = transform * corners[i];
	}

	glm::vec4 newMinimum = transformedCorners[0];
	glm::vec4 newMaximum = transformedCorners[0];

	for (uint32_t i = 1; i < 8; i++)
	{
		newMinimum.x = glm::min(newMinimum.x, transformedCorners[i].x);
		newMinimum.y = glm::min(newMinimum.y, transformedCorners[i].y);
		newMinimum.z = glm::min(newMinimum.z, transformedCorners[i].z);

		newMaximum.x = glm::max(newMaximum.x, transformedCorners[i].x);
		newMaximum.y = glm::max(newMaximum.y, transformedCorners[i].y);
		newMaximum.z = glm::max(newMaximum.z, transformedCorners[i].z);
	}

	return Aabb(newMinimum, newMaximum);
}

Aabb operator *(const glm::mat4& transform, const Aabb& aabb)
{
	glm::vec4 transformedCorners[8];
	for (uint32_t i = 0; i < 8; i++)
	{
		transformedCorners[i] = transform * aabb.getCorner(i);
	}

	glm::vec4 newMinimum = transformedCorners[0];
	glm::vec4 newMaximum = transformedCorners[0];

	for (uint32_t i = 1; i < 8; i++)
	{
		newMinimum.x = glm::min(newMinimum.x, transformedCorners[i].x);
		newMinimum.y = glm::min(newMinimum.y, transformedCorners[i].y);
		newMinimum.z = glm::min(newMinimum.z, transformedCorners[i].z);

		newMaximum.x = glm::max(newMaximum.x, transformedCorners[i].x);
		newMaximum.y = glm::max(newMaximum.y, transformedCorners[i].y);
		newMaximum.z = glm::max(newMaximum.z, transformedCorners[i].z);
	}

	return Aabb(newMinimum, newMaximum);
}
