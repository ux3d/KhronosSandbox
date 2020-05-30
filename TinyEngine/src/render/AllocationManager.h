#ifndef RENDER_ALLOCATIONMANAGER_H_
#define RENDER_ALLOCATIONMANAGER_H_

#include <cstdint>

#include "../composite/Composite.h"
#include "../gltf/GLTF.h"
#include "../resource/Resource.h"

class AllocationManager {

private:

	ResourceManager resourceManager;

public:

	AllocationManager();

	~AllocationManager();

	//

	ResourceManager& getResourceManager();

	BufferViewResource* getBufferViewResource(const BufferView* bufferView);
	TextureResource* getTextureResource(const Texture* texture);
	MaterialResource* getMaterialResource(const Material* material);
	PrimitiveResource* getPrimitiveResource(const Primitive* primitive);
	SceneResource* getSceneResource(const Scene* scene);
	WorldResource* getWorldResource(const GLTF* glTF);

	//

	bool initBufferView(const BufferView& bufferView, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, bool useRaytrace = false);

	bool initMaterial(const Material& material, VkPhysicalDevice physicalDevice, VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings);

	bool initPrimitive(const Primitive& primitive, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, uint32_t width, uint32_t height, VkRenderPass renderPass, VkSampleCountFlagBits samples, const VkDescriptorSetLayout* pSetLayouts, VkCullModeFlags cullMode, bool useRaytrace = false);

	bool initScene(const Scene& scene, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImageView imageView, bool useRaytrace = false);

	void terminate(VkDevice device);

};

#endif /* RENDER_ALLOCATIONMANAGER_H_ */
