#ifndef GLTF_HELPERLOADER_H_
#define GLTF_HELPERLOADER_H_

#include "../../generic/gltf/GLTF.h"

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

	bool initBuffers(GLTF& glTF);

	bool initBufferViews(GLTF& glTF, bool useRaytrace);

	bool initAccessors(GLTF& glTF);

	bool initImages(GLTF& glTF, const std::string& path);

	bool initSamplers(GLTF& glTF);

	bool initTextures(GLTF& glTF);

	bool initMaterials(GLTF& glTF);

	bool initMeshes(GLTF& glTF, bool useRaytrace);

	bool initNodes(GLTF& glTF, bool useRaytrace);

	bool initScenes(GLTF& glTF, bool useRaytrace);

public:

	HelperLoader(uint32_t width, uint32_t height, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkRenderPass renderPass, VkSampleCountFlagBits samples, VkImageView imageView = VK_NULL_HANDLE);

	bool open(GLTF& glTF, const std::string& filename, const std::string& diffuseFilename, bool useRaytrace = false);

};

#endif /* GLTF_HELPERLOADER_H_ */
