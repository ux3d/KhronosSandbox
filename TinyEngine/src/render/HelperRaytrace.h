#ifndef RENDER_HELPERRAYTRACE_H_
#define RENDER_HELPERRAYTRACE_H_

#include <cstdint>

#include "../common/Common.h"
#include "../gltf/GLTF.h"

#include "../render/AllocationManager.h"

class HelperRaytrace {
public:

	static void draw(AllocationManager& allocationManager, const Scene& scene, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, uint32_t width, uint32_t height);

	static void draw(AllocationManager& allocationManager, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, uint32_t width, uint32_t height);

};

#endif /* RENDER_HELPERRAYTRACE_H_ */
