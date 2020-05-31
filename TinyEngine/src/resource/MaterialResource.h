#ifndef RESOURCE_MATERIALRESOURCE_H_
#define RESOURCE_MATERIALRESOURCE_H_

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "../composite/Composite.h"

struct MaterialResource {
	// Mapper helper

	uint32_t alphaMode = 0;

	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

	uint32_t binding = 0;

	// Rasterize helper

	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

	std::vector<VkDescriptorImageInfo> descriptorImageInfos;
	VkDescriptorBufferInfo descriptorBufferInfo = {};

	UniformBufferResource uniformBufferResource = {};

	std::map<std::string, std::string> macros;

};

#endif /* RESOURCE_MATERIALRESOURCE_H_ */
