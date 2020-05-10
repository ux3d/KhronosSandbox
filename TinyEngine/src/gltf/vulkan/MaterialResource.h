#ifndef GLTF_MATERIALRESOURCE_H_
#define GLTF_MATERIALRESOURCE_H_

#include "../../TinyEngine.h"

class MaterialResource {
public:

	// Rasterize helper

	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet;

	std::vector<VkDescriptorImageInfo> descriptorImageInfos;
	VkDescriptorBufferInfo descriptorBufferInfo = {};

	UniformBufferResource uniformBufferResource;

	std::map<std::string, std::string> macros;

};

#endif /* GLTF_MATERIALRESOURCE_H_ */
