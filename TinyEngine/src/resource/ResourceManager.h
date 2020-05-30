#ifndef RESOURCE_RESOURCEMANAGER_H_
#define RESOURCE_RESOURCEMANAGER_H_

#include <cstdint>
#include <map>

#include "../composite/Composite.h"

#include "SharedDataResource.h"
#include "MaterialResource.h"
#include "PrimitiveResource.h"
#include "GroupResource.h"
#include "InstanceResource.h"
#include "WorldResource.h"

class ResourceManager {

private:

	std::map<uint64_t, SharedDataResource> sharedDataResources;
	std::map<uint64_t, TextureResource> textureResources;
	std::map<uint64_t, MaterialResource> materialResources;
	std::map<uint64_t, PrimitiveResource> primitiveResources;
	std::map<uint64_t, GroupResource> groupResources;
	std::map<uint64_t, InstanceResource> instanceResources;
	std::map<uint64_t, WorldResource> worldResources;

	void terminate(SharedDataResource& sharedDataResource, VkDevice device);
	void terminate(TextureResource& textureResource, VkDevice device);
	void terminate(MaterialResource& materialResource, VkDevice device);
	void terminate(PrimitiveResource& primitiveResource, VkDevice device);
	void terminate(GroupResource& groupResource, VkDevice device);
	void terminate(InstanceResource& instanceResource, VkDevice device);
	void terminate(WorldResource& worldResource, VkDevice device);

public:

	ResourceManager();

	~ResourceManager();

	//

	VkBuffer getBuffer(uint64_t bufferViewHandle);

	//

	SharedDataResource* getSharedDataResource(uint64_t sharedDataHandle);
	TextureResource* getTextureResource(uint64_t textureHandle);
	MaterialResource* getMaterialResource(uint64_t materialHandle);
	PrimitiveResource* getPrimitiveResource(uint64_t primitiveHandle);
	GroupResource* getGroupResource(uint64_t sceneHandle);
	InstanceResource* getInstanceResource(uint64_t sceneHandle);
	WorldResource* getWorldResource(uint64_t worldHandle);

	//

	bool createSharedDataResource(uint64_t externalHandle, VkDeviceSize size, const void* data, VkBufferUsageFlags usage, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool);
	bool createTextureResource(uint64_t externalHandle);
	bool createMaterialResource(uint64_t externalHandle, uint32_t alphaMode, const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings, VkDevice device);
	bool createPrimitiveResource(uint64_t externalHandle);
	bool createGroupResource(uint64_t externalHandle);
	bool createInstanceResource(uint64_t externalHandle);
	bool createWorldResource(uint64_t externalHandle);

	//

	bool deleteSharedDataResource(uint64_t sharedDataHandle, VkDevice device);
	bool deleteTextureResource(uint64_t textureHandle, VkDevice device);
	bool deleteMaterialResource(uint64_t materialHandle, VkDevice device);
	bool deletePrimitiveResource(uint64_t primitiveHanlde, VkDevice device);
	bool deleteGroupResource(uint64_t groupHandle, VkDevice device);
	bool deleteInstanceResource(uint64_t instanceHandle, VkDevice device);
	bool deleteWorldResource(uint64_t worldHandle, VkDevice device);

	//

	void terminate(VkDevice device);

};

#endif /* RESOURCE_RESOURCEMANAGER_H_ */
