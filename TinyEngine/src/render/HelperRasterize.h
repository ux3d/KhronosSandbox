#ifndef RENDER_HELPERRASTERIZE_H_
#define RENDER_HELPERRASTERIZE_H_

#include <cstdint>

#include "../gltf/GLTF.h"
#include "../resource/ResourceManager.h"

enum DrawMode {
	ALL,
	OPAQUE,
	TRANSPARENT
};

class HelperRasterize {
public:

	static void draw(ResourceManager& resourceManager, WorldResource& worldResource, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode);
};

#endif /* RENDER_HELPERRASTERIZE_H_ */
