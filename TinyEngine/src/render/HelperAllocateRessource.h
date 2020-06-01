#ifndef RENDER_HELPERALLOCATERESSOURCE_H_
#define RENDER_HELPERALLOCATERESSOURCE_H_

#include <cstdint>
#include <vector>

#include "../composite/Composite.h"
#include "../gltf/GLTF.h"
#include "../resource/Resource.h"

class HelperAllocateResource {

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

	bool initBufferViews(const GLTF& glTF, bool useRaytrace);

	bool initAccessors(const GLTF& glTF, bool useRaytrace);

	bool initTextures(const GLTF& glTF, bool useRaytrace);

	bool initMaterials(const GLTF& glTF, bool useRaytrace);

	bool initMeshes(const GLTF& glTF, bool useRaytrace);

	bool initNodes(const GLTF& glTF, bool useRaytrace);

	bool initScene(const GLTF& glTF, bool useRaytrace);

public:

	HelperAllocateResource(ResourceManager& resourceManager, uint32_t width, uint32_t height, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkRenderPass renderPass, VkSampleCountFlagBits samples, VkImageView imageView = VK_NULL_HANDLE);

	bool allocate(const GLTF& glTF, const std::string& diffuseFilename, bool useRaytrace = false);

	//

	uint64_t getBufferHandle(const Accessor& accessor);

	ResourceManager& getResourceManager();

	bool createSharedDataResource(const BufferView& bufferView, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, bool useRaytrace);

	bool finalizePrimitive(const Primitive& primitive, const GLTF& glTF, std::map<std::string, std::string>& macros, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, uint32_t width, uint32_t height, VkRenderPass renderPass, VkSampleCountFlagBits samples, const VkDescriptorSetLayout* pSetLayouts, VkCullModeFlags cullMode, bool useRaytrace = false);

	bool finalizeWorld(const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImageView imageView, bool useRaytrace = false);

	void terminate(VkDevice device);

};

#endif /* RENDER_HELPERALLOCATERESSOURCE_H_ */
