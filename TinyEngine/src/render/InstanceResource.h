#ifndef RENDER_INSTANCERESOURCE_H_
#define RENDER_INSTANCERESOURCE_H_

#include <cstdint>
#include <vector>

#include "../composite/Composite.h"
#include "../math/Math.h"

#include "BaseResource.h"

struct InstanceContainer {

	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

	VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;

	//

	std::vector<uint32_t> dynamicOffsets;

};

struct InstanceResource : BaseResource {

	uint64_t groupHandle;

	glm::mat4 worldMatrix = glm::mat4(1.0f);

	std::vector<InstanceContainer> instanceContainers;

	uint64_t weightsHandle = 0;

	uint64_t jointMatricesHandle = 0;
	uint32_t jointMatricesCount = 0;

};

#endif /* RENDER_INSTANCERESOURCE_H_ */
