#ifndef GLTF_HELPERALLOCATERESSOURCE_H_
#define GLTF_HELPERALLOCATERESSOURCE_H_

#include "../GLTF.h"

#include "ResourceManager.h"

class HelperAllocateResource {

private:

	uint32_t width;
	uint32_t height;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue queue;
	VkCommandPool commandPool;
	VkRenderPass renderPass;
	VkSampleCountFlagBits samples;
	VkImageView imageView;

	bool initBuffers(ResourceManager& resourceManager, const GLTF& glTF);

	bool initBufferViews(ResourceManager& resourceManager, const GLTF& glTF, bool useRaytrace);

	bool initAccessors(ResourceManager& resourceManager, const GLTF& glTF);

	bool initImages(ResourceManager& resourceManager, const GLTF& glTF);

	bool initSamplers(ResourceManager& resourceManager, const GLTF& glTF);

	bool initTextures(ResourceManager& resourceManager, const GLTF& glTF);

	bool initMaterials(ResourceManager& resourceManager, const GLTF& glTF);

	bool initMeshes(ResourceManager& resourceManager, const GLTF& glTF, bool useRaytrace);

	bool initNodes(ResourceManager& resourceManager, const GLTF& glTF, bool useRaytrace);

	bool initScenes(ResourceManager& resourceManager, const GLTF& glTF, bool useRaytrace);

public:

	HelperAllocateResource(uint32_t width, uint32_t height, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkRenderPass renderPass, VkSampleCountFlagBits samples, VkImageView imageView = VK_NULL_HANDLE);

	bool open(ResourceManager& resourceManager, GLTF& glTF, const std::string& diffuseFilename, bool useRaytrace = false);

};

#endif /* GLTF_HELPERALLOCATERESSOURCE_H_ */
