#ifndef RENDER_HELPERRAYTRACE_H_
#define RENDER_HELPERRAYTRACE_H_

#include <cstdint>

#include "../common/Common.h"

#include "ResourceManager.h"

class HelperRaytrace {
public:

	static void draw(ResourceManager& resourceManager, VkCommandBuffer commandBuffer, uint32_t frameIndex, uint32_t width, uint32_t height);

};

#endif /* RENDER_HELPERRAYTRACE_H_ */