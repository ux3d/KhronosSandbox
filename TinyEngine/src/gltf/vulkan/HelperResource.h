#ifndef GLTF_HELPERRESOURCE_H_
#define GLTF_HELPERRESOURCE_H_

#include "../GLTF.h"

class HelperResource {
public:

	static bool initBufferView(BufferView& bufferView, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, bool useRaytrace = false);

	static bool initMaterial(Material& material, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings);

	static bool initPrimitive(Primitive& primitive, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, uint32_t width, uint32_t height, VkRenderPass renderPass, VkSampleCountFlagBits samples, const VkDescriptorSetLayout* pSetLayouts, VkCullModeFlags cullMode, bool useRaytrace = false);

	static bool initScene(Scene& scene, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImageView imageView, bool useRaytrace = false);

	static void terminate(GLTF& glTF, VkDevice device);

};

#endif /* GLTF_HELPERRESOURCE_H_ */
