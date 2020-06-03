#ifndef BUILDER_WORLDBUILDER_H_
#define BUILDER_WORLDBUILDER_H_

#include <cstdint>
#include <vector>

#include "../composite/Composite.h"
#include "../gltf/GLTF.h"

#include "../render/Render.h"

class WorldBuilder {

private:

	ResourceManager& resourceManager;

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

	bool buildBufferViews(const GLTF& glTF, bool useRaytrace);

	bool buildAccessors(const GLTF& glTF, bool useRaytrace);

	bool buildTextures(const GLTF& glTF, bool useRaytrace);

	bool buildMaterials(const GLTF& glTF, bool useRaytrace);

	bool buildMeshes(const GLTF& glTF, bool useRaytrace);

	bool buildNodes(const GLTF& glTF, bool useRaytrace);

	bool buildScene(const GLTF& glTF, bool useRaytrace);

	uint64_t getBufferHandle(const Accessor& accessor);

	bool createSharedDataResource(const BufferView& bufferView, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, bool useRaytrace);
	bool createSharedDataResource(VkDeviceSize size, const void* data, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, bool useRaytrace);

	bool finalizeWorld(const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImageView imageView, bool useRaytrace = false);

public:

	WorldBuilder(ResourceManager& resourceManager, uint32_t width, uint32_t height, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkRenderPass renderPass, VkSampleCountFlagBits samples, VkImageView imageView = VK_NULL_HANDLE);

	bool build(const GLTF& glTF, const std::string& diffuseFilename, bool useRaytrace = false);

	void terminate(VkDevice device);

};

#endif /* BUILDER_WORLDBUILDER_H_ */
