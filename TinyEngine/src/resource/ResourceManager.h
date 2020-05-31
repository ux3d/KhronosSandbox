#ifndef RESOURCE_RESOURCEMANAGER_H_
#define RESOURCE_RESOURCEMANAGER_H_

#include <cstdint>
#include <map>
#include <vector>

#include "../composite/Composite.h"

#include "SharedDataResource.h"
#include "TextureDataResource.h"
#include "MaterialResource.h"
#include "GeometryResource.h"
#include "GeometryModelResource.h"
#include "GroupResource.h"
#include "InstanceResource.h"
#include "WorldResource.h"

class ResourceManager {

private:

	std::map<uint64_t, SharedDataResource> sharedDataResources;
	std::map<uint64_t, TextureDataResource> textureResources;
	std::map<uint64_t, MaterialResource> materialResources;
	std::map<uint64_t, GeometryResource> geometryResources;
	std::map<uint64_t, GeometryModelResource> geometryModelResources;
	std::map<uint64_t, GroupResource> groupResources;
	std::map<uint64_t, InstanceResource> instanceResources;
	std::map<uint64_t, WorldResource> worldResources;

	void terminate(SharedDataResource& sharedDataResource, VkDevice device);
	void terminate(TextureDataResource& textureResource, VkDevice device);
	void terminate(MaterialResource& materialResource, VkDevice device);
	void terminate(GeometryResource& geometryResource, VkDevice device);
	void terminate(GeometryModelResource& geometryModelResource, VkDevice device);
	void terminate(GroupResource& groupResource, VkDevice device);
	void terminate(InstanceResource& instanceResource, VkDevice device);
	void terminate(WorldResource& worldResource, VkDevice device);

	SharedDataResource* getSharedDataResource(uint64_t sharedDataHandle);

public:

	ResourceManager();

	~ResourceManager();

	//

	VkBuffer getBuffer(uint64_t bufferViewHandle);

	// TODO: Should become private.

	TextureDataResource* getTextureResource(uint64_t textureHandle);
	MaterialResource* getMaterialResource(uint64_t materialHandle);
	GeometryResource* getGeometryResource(uint64_t geometryHandle);
	GeometryModelResource* getGeometryModelResource(uint64_t geometryModelHandle);
	GroupResource* getGroupResource(uint64_t groupHandle);
	InstanceResource* getInstanceResource(uint64_t instanceHandle);
	WorldResource* getWorldResource(uint64_t worldHandle);

	// One time setup before finalization.

	bool sharedDataSetData(uint64_t sharedDataHandle, VkDeviceSize size, const void* data);
	bool sharedDataSetUsage(uint64_t sharedDataHandle, VkBufferUsageFlags usage);

	bool textureResourceSetCreateInformation(uint64_t textureHandle, const TextureResourceCreateInfo& textureResourceCreateInfo);

	bool materialResourceSetMaterialParameters(uint64_t materialHandle, const MaterialUniformBuffer& materialUniformBuffer, VkPhysicalDevice physicalDevice, VkDevice device);
	bool materialResourceSetTextureResource(uint64_t materialHandle, uint64_t textureHandle, uint32_t texCoord, const std::string& prefix);

	bool geometryResourceSetAttributesCount(uint64_t geometryHandle, uint32_t attributesCount);
	bool geometryResourceSetPrimitiveResource(uint64_t geometryHandle, uint32_t typeCount, const std::string& prefix, std::map<std::string, std::string>& macros, VkFormat format, uint32_t stride, VkBuffer buffer, VkDeviceSize offset);

	bool geometryModelResourceSetGeometryResource(uint64_t geometryModelHandle, uint64_t geometryHandle);
	bool geometryModelResourceSetMaterialResource(uint64_t geometryModelHandle, uint64_t materialHandle);

	bool groupResourceAddGeometryModelResource(uint64_t groupHandle, uint64_t geometryModelHandle);

	bool instanceResourceSetWorldMatrix(uint64_t instanceHandle, const glm::mat4& worldMatrix);
	bool instanceResourceSetGroupResource(uint64_t instanceHandle, uint64_t groupHandle);

	// Finalization and setup not allowed anymore.

	bool sharedDataResourceFinalize(uint64_t sharedDataHandle, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool);
	bool textureResourceFinalize(uint64_t textureHandle, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool);
	bool materialResourceFinalize(uint64_t materialHandle, VkDevice device);
	bool geometryResourceFinalize(uint64_t geometryHandle);
	bool geometryModelResourceFinalize(uint64_t geometryModelHandle);
	bool groupResourceFinalize(uint64_t groupHandle);
	bool instanceResourceFinalize(uint64_t instanceHandle);
	bool worldResourceFinalize(uint64_t worldHandle);

	// Update also after finalization.

	bool instanceResourceUpdateWorldMatrix(uint64_t instanceHandle, const glm::mat4& worldMatrix);

	// Delete and free data.

	bool sharedDataResourceDelete(uint64_t sharedDataHandle, VkDevice device);
	bool textureResourceDelete(uint64_t textureHandle, VkDevice device);
	bool materialResourceDelete(uint64_t materialHandle, VkDevice device);
	bool geometryResourceDelete(uint64_t geometryHanlde, VkDevice device);
	bool geometryModelResourceDelete(uint64_t geometryModelHandle, VkDevice device);
	bool groupResourceDelete(uint64_t groupHandle, VkDevice device);
	bool instanceResourceDelete(uint64_t instanceHandle, VkDevice device);
	bool worldResourceDelete(uint64_t worldHandle, VkDevice device);

	//

	void terminate(VkDevice device);

};

#endif /* RESOURCE_RESOURCEMANAGER_H_ */
