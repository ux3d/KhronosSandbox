#ifndef RESOURCE_HELPERRASTERIZE_H_
#define RESOURCE_HELPERRASTERIZE_H_

#include <cstdint>

#include "ResourceManager.h"

enum DrawMode {
	ALL,
	OPAQUE,
	TRANSPARENT
};

class HelperRasterize {
public:

	static void draw(ResourceManager& resourceManager, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode);
};

#endif /* RESOURCE_HELPERRASTERIZE_H_ */
