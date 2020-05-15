#ifndef RENDER_MATERIALRESOURCE_H_
#define RENDER_MATERIALRESOURCE_H_

#include <map>
#include <string>
#include <vector>

#include "../composite/Composite.h"

class MaterialResource {
public:

	// Rasterize helper

	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

	std::vector<VkDescriptorImageInfo> descriptorImageInfos;
	VkDescriptorBufferInfo descriptorBufferInfo = {};

	UniformBufferResource uniformBufferResource = {};

	std::map<std::string, std::string> macros;

};

#endif /* RENDER_MATERIALRESOURCE_H_ */
