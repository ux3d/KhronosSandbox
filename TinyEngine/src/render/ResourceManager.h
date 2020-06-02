#ifndef RENDER_RESOURCEMANAGER_H_
#define RENDER_RESOURCEMANAGER_H_

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
#include "LightResource.h"
#include "CameraResource.h"
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
	std::map<uint64_t, LightResource> lightResources;
	std::map<uint64_t, CameraResource> cameraResources;
	WorldResource worldResource;

	void terminate(SharedDataResource& sharedDataResource, VkDevice device);
	void terminate(TextureDataResource& textureResource, VkDevice device);
	void terminate(MaterialResource& materialResource, VkDevice device);
	void terminate(GeometryResource& geometryResource, VkDevice device);
	void terminate(GeometryModelResource& geometryModelResource, VkDevice device);
	void terminate(GroupResource& groupResource, VkDevice device);
	void terminate(InstanceResource& instanceResource, VkDevice device);
	void terminate(LightResource& lightResource, VkDevice device);
	void terminate(CameraResource& cameraResource, VkDevice device);
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
	LightResource* getLightResource(uint64_t lightHandle);
	CameraResource* getCameraResource(uint64_t cameraHandle);
	WorldResource* getWorldResource();

	// One time setup before finalization.

	bool sharedDataSetData(uint64_t sharedDataHandle, VkDeviceSize size, const void* data);
	bool sharedDataSetUsage(uint64_t sharedDataHandle, VkBufferUsageFlags usage);

	bool textureResourceSetCreateInformation(uint64_t textureHandle, const TextureResourceCreateInfo& textureResourceCreateInfo);

	bool materialResourceSetMaterialParameters(uint64_t materialHandle, const MaterialUniformBuffer& materialUniformBuffer, VkPhysicalDevice physicalDevice, VkDevice device);
	bool materialResourceSetTextureResource(uint64_t materialHandle, uint64_t textureHandle, uint32_t texCoord, const std::string& prefix);

	bool geometryResourceSetAttributesCount(uint64_t geometryHandle, uint32_t attributesCount);
	bool geometryResourceSetAttribute(uint64_t geometryHandle, uint32_t count, uint32_t typeCount, const std::string& prefix, VkFormat format, uint32_t stride, VkBuffer buffer, VkDeviceSize offset);

	bool geometryModelResourceSetGeometryResource(uint64_t geometryModelHandle, uint64_t geometryHandle);
	bool geometryModelResourceSetMaterialResource(uint64_t geometryModelHandle, uint64_t materialHandle);
	bool geometryModelResourceSetVertexCount(uint64_t geometryModelHandle, uint32_t verticesCount);
	bool geometryModelResourceSetIndices(uint64_t geometryModelHandle, uint32_t indicesCount, VkIndexType indexType, VkBuffer indexBuffer, uint32_t indexOffset);
	bool geometryModelResourceSetTargetData(uint64_t geometryModelHandle, const std::string& targetName, uint64_t sharedDataHandle);

	bool groupResourceAddGeometryModelResource(uint64_t groupHandle, uint64_t geometryModelHandle);

	bool instanceResourceSetWorldMatrix(uint64_t instanceHandle, const glm::mat4& worldMatrix);
	bool instanceResourceSetGroupResource(uint64_t instanceHandle, uint64_t groupHandle);

	bool lightResourceSetEnvironmentLight(uint64_t lightHandle, const std::string& environment);

	bool worldResourceAddInstanceResource(uint64_t instanceHandle);
	bool worldResourceSetLightResource(uint64_t lightHandle);
	bool worldResourceSetCameraResource(uint64_t cameraHandle);

	// Finalization and setup not allowed anymore.

	bool sharedDataResourceFinalize(uint64_t sharedDataHandle, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool);
	bool textureResourceFinalize(uint64_t textureHandle, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool);
	bool materialResourceFinalize(uint64_t materialHandle, VkDevice device);
	bool geometryResourceFinalize(uint64_t geometryHandle);
	bool geometryModelResourceFinalize(uint64_t geometryModelHandle);
	bool groupResourceFinalize(uint64_t groupHandle);
	bool instanceResourceFinalize(uint64_t instanceHandle);
	bool lightResourceFinalize(uint64_t lightHandle, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool);
	bool cameraResourceFinalize(uint64_t cameraHandle);
	bool worldResourceFinalize(VkDevice device);

	// Getters

	bool worldResourceGetCameraResource(uint64_t& cameraHandle);

	// Update also after finalization.

	bool instanceResourceUpdateWorldMatrix(uint64_t instanceHandle, const glm::mat4& worldMatrix);

	bool cameraResourceUpdateProjectionMatrix(uint64_t cameraHandle, const glm::mat4& projectionMatrix);
	bool cameraResourceUpdateViewMatrix(uint64_t cameraHandle, const glm::mat4& viewMatrix);

	// Delete and free data.

	bool sharedDataResourceDelete(uint64_t sharedDataHandle, VkDevice device);
	bool textureResourceDelete(uint64_t textureHandle, VkDevice device);
	bool materialResourceDelete(uint64_t materialHandle, VkDevice device);
	bool geometryResourceDelete(uint64_t geometryHanlde, VkDevice device);
	bool geometryModelResourceDelete(uint64_t geometryModelHandle, VkDevice device);
	bool groupResourceDelete(uint64_t groupHandle, VkDevice device);
	bool instanceResourceDelete(uint64_t instanceHandle, VkDevice device);
	bool lightResourceDelete(uint64_t lightHandle, VkDevice device);
	bool cameraResourceDelete(uint64_t cameraHandle, VkDevice device);
	bool worldResourceDelete(VkDevice device);

	//

	void terminate(VkDevice device);

};

#endif /* RENDER_RESOURCEMANAGER_H_ */
