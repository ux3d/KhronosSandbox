#ifndef GLTF_HELPERLOADER_H_
#define GLTF_HELPERLOADER_H_

#include "../GLTF.h"

#include "ResourceManager.h"

#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

class HelperLoader {

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

	tinygltf::Model model;

	bool initBuffers(ResourceManager& resourceManager, GLTF& glTF);

	bool initBufferViews(ResourceManager& resourceManager, GLTF& glTF, bool useRaytrace);

	bool initAccessors(ResourceManager& resourceManager, GLTF& glTF);

	bool initImages(ResourceManager& resourceManager, GLTF& glTF, const std::string& path);

	bool initSamplers(ResourceManager& resourceManager, GLTF& glTF);

	bool initTextures(ResourceManager& resourceManager, GLTF& glTF);

	bool initMaterials(ResourceManager& resourceManager, GLTF& glTF);

	bool initMeshes(ResourceManager& resourceManager, GLTF& glTF, bool useRaytrace);

	bool initNodes(ResourceManager& resourceManager, GLTF& glTF, bool useRaytrace);

	bool initScenes(ResourceManager& resourceManager, GLTF& glTF, bool useRaytrace);

public:

	HelperLoader(uint32_t width, uint32_t height, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkRenderPass renderPass, VkSampleCountFlagBits samples, VkImageView imageView = VK_NULL_HANDLE);

	bool open(ResourceManager& resourceManager, GLTF& glTF, const std::string& filename, const std::string& diffuseFilename, bool useRaytrace = false);

};

#endif /* GLTF_HELPERLOADER_H_ */
