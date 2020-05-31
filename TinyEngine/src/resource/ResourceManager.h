#ifndef RESOURCE_RESOURCEMANAGER_H_
#define RESOURCE_RESOURCEMANAGER_H_

#include <cstdint>
#include <map>

#include "../composite/Composite.h"

#include "SharedDataResource.h"
#include "MaterialResource.h"
#include "GeometryResource.h"
#include "GeometryModelResource.h"
#include "GroupResource.h"
#include "InstanceResource.h"
#include "WorldResource.h"

class ResourceManager {

private:

	std::map<uint64_t, SharedDataResource> sharedDataResources;
	std::map<uint64_t, TextureResource> textureResources;
	std::map<uint64_t, MaterialResource> materialResources;
	std::map<uint64_t, GeometryResource> geometryResources;
	std::map<uint64_t, GeometryModelResource> geometryModelResources;
	std::map<uint64_t, GroupResource> groupResources;
	std::map<uint64_t, InstanceResource> instanceResources;
	std::map<uint64_t, WorldResource> worldResources;

	void terminate(SharedDataResource& sharedDataResource, VkDevice device);
	void terminate(TextureResource& textureResource, VkDevice device);
	void terminate(MaterialResource& materialResource, VkDevice device);
	void terminate(GeometryResource& geometryResource, VkDevice device);
	void terminate(GeometryModelResource& geometryModelResource, VkDevice device);
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
	GeometryResource* getGeometryResource(uint64_t geometryHandle);
	GeometryModelResource* getGeometryModelResource(uint64_t geometryModelHandle);
	GroupResource* getGroupResource(uint64_t groupHandle);
	InstanceResource* getInstanceResource(uint64_t instanceHandle);
	WorldResource* getWorldResource(uint64_t worldHandle);

	//

	bool instanceResourceSetWorldMatrix(uint64_t instanceHandle, const glm::mat4& worldMatrix);
	bool instanceResourceSetGroupResource(uint64_t instanceHandle, uint64_t groupHandle);

	//

	bool finalizeSharedDataResource(uint64_t externalHandle, VkDeviceSize size, const void* data, VkBufferUsageFlags usage, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool);
	bool finalizeTextureResource(uint64_t externalHandle, const TextureResourceCreateInfo& textureResourceCreateInfo, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool);
	bool finalizeMaterialResource(uint64_t externalHandle, VkDevice device);
	bool finalizeGeometryResource(uint64_t externalHandle);
	bool finalizeGeometryModelResource(uint64_t externalHandle);
	bool finalizeGroupResource(uint64_t externalHandle);
	bool finalizeInstanceResource(uint64_t externalHandle);
	bool finalizeWorldResource(uint64_t externalHandle);

	//

	bool deleteSharedDataResource(uint64_t sharedDataHandle, VkDevice device);
	bool deleteTextureResource(uint64_t textureHandle, VkDevice device);
	bool deleteMaterialResource(uint64_t materialHandle, VkDevice device);
	bool deleteGeometryResource(uint64_t geometryHanlde, VkDevice device);
	bool deleteGeometryModelResource(uint64_t geometryModelHanlde, VkDevice device);
	bool deleteGroupResource(uint64_t groupHandle, VkDevice device);
	bool deleteInstanceResource(uint64_t instanceHandle, VkDevice device);
	bool deleteWorldResource(uint64_t worldHandle, VkDevice device);

	//

	void terminate(VkDevice device);

};

#endif /* RESOURCE_RESOURCEMANAGER_H_ */
