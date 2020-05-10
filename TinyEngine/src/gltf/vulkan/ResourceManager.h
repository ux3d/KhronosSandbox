#ifndef GLTF_RESOURCEMANAGER_H_
#define GLTF_RESOURCEMANAGER_H_

#include "../GLTF.h"

#include "BufferViewResource.h"
#include "MaterialResource.h"
#include "PrimitiveResource.h"
#include "SceneResource.h"
#include "GltfResource.h"

#include <map>

class ResourceManager {

private:

	std::map<BufferView*, BufferViewResource> bufferViewResources;
	std::map<Material*, MaterialResource> materialResources;
	std::map<Primitive*, PrimitiveResource> primitiveResources;
	std::map<Scene*, SceneResource> sceneResources;
	GltfResource gltfResource;

public:

	ResourceManager();

	~ResourceManager();

	bool initBufferView(BufferView& bufferView, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, bool useRaytrace = false);

	bool initMaterial(Material& material, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings);

	bool initPrimitive(Primitive& primitive, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, uint32_t width, uint32_t height, VkRenderPass renderPass, VkSampleCountFlagBits samples, const VkDescriptorSetLayout* pSetLayouts, VkCullModeFlags cullMode, bool useRaytrace = false);

	bool initScene(Scene& scene, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImageView imageView, bool useRaytrace = false);

	void terminate(GLTF& glTF, VkDevice device);

};

#endif /* GLTF_RESOURCEMANAGER_H_ */
