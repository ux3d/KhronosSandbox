#ifndef RENDER_HELPERALLOCATERESSOURCE_H_
#define RENDER_HELPERALLOCATERESSOURCE_H_

#include <cstdint>

#include "../common/Common.h"
#include "../gltf/GLTF.h"

#include "AllocationManager.h"

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

	bool initBuffers(AllocationManager& resourceManager, const GLTF& glTF, bool useRaytrace);

	bool initBufferViews(AllocationManager& resourceManager, const GLTF& glTF, bool useRaytrace);

	bool initAccessors(AllocationManager& resourceManager, const GLTF& glTF, bool useRaytrace);

	bool initImages(AllocationManager& resourceManager, const GLTF& glTF, bool useRaytrace);

	bool initSamplers(AllocationManager& resourceManager, const GLTF& glTF, bool useRaytrace);

	bool initTextures(AllocationManager& resourceManager, const GLTF& glTF, bool useRaytrace);

	bool initMaterials(AllocationManager& resourceManager, const GLTF& glTF, bool useRaytrace);

	bool initMeshes(AllocationManager& resourceManager, const GLTF& glTF, bool useRaytrace);

	bool initNodes(AllocationManager& resourceManager, const GLTF& glTF, bool useRaytrace);

	bool initScenes(AllocationManager& resourceManager, const GLTF& glTF, bool useRaytrace);

public:

	HelperAllocateResource(uint32_t width, uint32_t height, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkRenderPass renderPass, VkSampleCountFlagBits samples, VkImageView imageView = VK_NULL_HANDLE);

	bool allocate(AllocationManager& resourceManager, const GLTF& glTF, const std::string& diffuseFilename, bool useRaytrace = false);

};

#endif /* RENDER_HELPERALLOCATERESSOURCE_H_ */
