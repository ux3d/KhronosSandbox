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

	std::map<const BufferView*, BufferViewResource> bufferViewResources;
	std::map<const Material*, MaterialResource> materialResources;
	std::map<const Primitive*, PrimitiveResource> primitiveResources;
	std::map<const Scene*, SceneResource> sceneResources;
	GltfResource gltfResource;

public:

	ResourceManager();

	~ResourceManager();

	//

	BufferViewResource* getBufferViewResource(const BufferView* bufferView);
	MaterialResource* getMaterialResource(const Material* material);
	PrimitiveResource* getPrimitiveResource(const Primitive* primitive);
	SceneResource* getSceneResource(const Scene* scene);
	GltfResource* getGltfResource();

	//

	bool initBufferView(const BufferView& bufferView, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, bool useRaytrace = false);

	bool initMaterial(const Material& material, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings);

	bool initPrimitive(const Primitive& primitive, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, uint32_t width, uint32_t height, VkRenderPass renderPass, VkSampleCountFlagBits samples, const VkDescriptorSetLayout* pSetLayouts, VkCullModeFlags cullMode, bool useRaytrace = false);

	bool initScene(const Scene& scene, const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImageView imageView, bool useRaytrace = false);

	void terminate(GLTF& glTF, VkDevice device);

};

#endif /* GLTF_RESOURCEMANAGER_H_ */
