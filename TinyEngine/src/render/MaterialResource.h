#ifndef RENDER_MATERIALRENDER_H_
#define RENDER_MATERIALRENDER_H_

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "../composite/Composite.h"
#include "../math/Math.h"

#include "BaseResource.h"

struct MaterialParameters {
	glm::vec4 baseColorFactor = glm::vec4(1.0f);

	float metallicFactor = 1.0f;
	float roughnessFactor = 1.0f;
	float normalScale = 1.0f;
	float occlusionStrength = 1.0f;

	glm::vec3 emissiveFactor = glm::vec3(0.0f);
	uint32_t alphaMode = 0;

	float alphaCutoff = 0.5f;
	bool doubleSided = false;
};

struct MaterialResource : BaseResource {

	MaterialParameters materialParameters = {};

	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

	std::vector<VkDescriptorImageInfo> descriptorImageInfos;
	std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;

	UniformBufferResource uniformBufferResource = {};

	std::map<std::string, std::string> macros;

};

#endif /* RENDER_MATERIALRENDER_H_ */
