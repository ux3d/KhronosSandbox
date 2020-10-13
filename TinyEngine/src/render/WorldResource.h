#ifndef RENDER_WORLDRENDER_H_
#define RENDER_WORLDRENDER_H_

#include <cstdint>
#include <vector>

#include "../composite/Composite.h"
#include "../math/Math.h"

#include "BaseResource.h"

struct ViewProjectionUniformPushConstant {
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
};

struct ModelViewProjectionUniformPushConstant {
	ViewProjectionUniformPushConstant viewProjection = {};
	glm::mat4 world = glm::mat4(1.0f);
};

struct WorldResource : BaseResource {

	// Mapper helper

	std::vector<uint64_t> instanceHandles;

	uint64_t lightHandle;

	uint64_t cameraHandle;

	std::vector<VkDescriptorImageInfo> descriptorImageInfoTextures;
	std::vector<RaytraceMaterialUniformBuffer> materialBuffers;

	// Rasterize helper

	ViewProjectionUniformPushConstant viewProjection = {};

};

#endif /* RENDER_WORLDRENDER_H_ */
