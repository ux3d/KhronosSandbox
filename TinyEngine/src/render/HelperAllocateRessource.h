#ifndef RENDER_HELPERALLOCATERESSOURCE_H_
#define RENDER_HELPERALLOCATERESSOURCE_H_

#include <cstdint>
#include <vector>

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

	std::vector<uint64_t> textureHandles;
	std::vector<uint64_t> materialHandles;
	std::vector<uint64_t> meshHandles;
	std::vector<uint64_t> nodeHandles;
	uint64_t glTFHandle = 0;

	bool initBuffers(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace);

	bool initBufferViews(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace);

	bool initAccessors(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace);

	bool initImages(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace);

	bool initSamplers(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace);

	bool initTextures(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace);

	bool initMaterials(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace);

	bool initMeshes(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace);

	bool initNodes(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace);

public:

	HelperAllocateResource(uint32_t width, uint32_t height, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkRenderPass renderPass, VkSampleCountFlagBits samples, VkImageView imageView = VK_NULL_HANDLE);

	bool allocate(AllocationManager& allocationManager, const GLTF& glTF, const std::string& diffuseFilename, bool useRaytrace = false);

};

#endif /* RENDER_HELPERALLOCATERESSOURCE_H_ */
