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

struct UniformPushConstant {
	ViewProjectionUniformPushConstant viewProjection = {};
	glm::mat4 world = glm::mat4(1.0f);

	uint32_t verticesCount = 0;

	uint32_t targetsCount = 0;
};

struct WorldResource : BaseResource {

	std::vector<uint64_t> instanceHandles;

	uint64_t lightHandle;

	uint64_t cameraHandle;

	std::vector<VkDescriptorImageInfo> descriptorImageInfoTextures;

	ViewProjectionUniformPushConstant viewProjection = {};

};

#endif /* RENDER_WORLDRENDER_H_ */
