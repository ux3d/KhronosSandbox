#ifndef RENDER_RENDERMANAGER_H_
#define RENDER_RENDERMANAGER_H_

#include <cstdint>
#include <map>
#include <vector>

#include "../common/Common.h"

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

enum DrawMode {
	ALL,
	OPAQUE,
	TRANSPARENT
};

class RenderManager {

private:

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue queue = VK_NULL_HANDLE;
	VkCommandPool commandPool = VK_NULL_HANDLE;

	uint32_t width = 0;
	uint32_t height = 0;

	bool useRaytrace = false;

	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

	VkImageView imageView = VK_NULL_HANDLE;

	uint64_t handles = 0;

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

	SharedDataResource* getSharedData(uint64_t sharedDataHandle);
	TextureDataResource* getTexture(uint64_t textureHandle);
	MaterialResource* getMaterial(uint64_t materialHandle);
	GeometryResource* getGeometry(uint64_t geometryHandle);
	GeometryModelResource* getGeometryModel(uint64_t geometryModelHandle);
	GroupResource* getGroup(uint64_t groupHandle);
	InstanceResource* getInstance(uint64_t instanceHandle);
	LightResource* getLight(uint64_t lightHandle);
	CameraResource* getCamera(uint64_t cameraHandle);
	WorldResource* getWorld();

	//

	VkBuffer getBuffer(uint64_t sharedDataHandle);

	bool sharedDataSetData(uint64_t sharedDataHandle, VkDeviceSize size, const void* data, VkBufferUsageFlags usage);

public:

	RenderManager();

	~RenderManager();

	bool renderSetupVulkan(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool);

	bool renderSetDimension(uint32_t width, uint32_t height);

	bool renderUseRaytrace(bool useRaytrace);

	bool renderRasterizeSetRenderPass(VkRenderPass renderPass);
	bool renderRasterizeSetSamples(VkSampleCountFlagBits samples);

	bool renderRaytraceSetImageView(VkImageView imageView);

	bool renderRaytraceUpdateSettings(uint32_t maxDepth, uint32_t specularSamples, uint32_t diffuseSamples);

	// Resources need to be created.

	bool sharedDataCreate(uint64_t& sharedDataHandle);
	bool textureCreate(uint64_t& textureHandle);
	bool materialCreate(uint64_t& materialHandle);
	bool geometryCreate(uint64_t& geometryHandle);
	bool geometryModelCreate(uint64_t& geometryModelHandle);
	bool groupCreate(uint64_t& groupHandle);
	bool instanceCreate(uint64_t& instanceHandle);
	bool lightCreate(uint64_t& lightHandle);
	bool cameraCreate(uint64_t& cameraHandle);
	bool worldCreate();

	// One time setup before finalization.

	bool sharedDataCreateVertexBuffer(uint64_t sharedDataHandle, VkDeviceSize size, const void* data);
	bool sharedDataCreateIndexBuffer(uint64_t sharedDataHandle, VkDeviceSize size, const void* data);
	bool sharedDataCreateStorageBuffer(uint64_t sharedDataHandle, VkDeviceSize size, const void* data);

	bool textureSetParameters(uint64_t textureHandle, const TextureResourceCreateInfo& textureResourceCreateInfo);

	bool materialSetParameters(uint64_t materialHandle, const MaterialParameters& materialParameters);
	bool materialSetTexture(uint64_t materialHandle, uint64_t textureHandle, const std::string& description, uint32_t texCoord = 0);

	bool geometrySetAttribute(uint64_t geometryHandle, uint64_t sharedDataHandle, const std::string& description, uint32_t count, VkFormat format, uint32_t stride, VkDeviceSize offset, VkDeviceSize range);
	bool geometrySetAttribute(uint64_t geometryHandle, uint64_t sharedDataHandle, const std::string& description, uint32_t count, VkFormat format);

	bool geometryModelSetGeometry(uint64_t geometryModelHandle, uint64_t geometryHandle);
	bool geometryModelSetMaterial(uint64_t geometryModelHandle, uint64_t materialHandle);
	bool geometryModelSetVertexCount(uint64_t geometryModelHandle, uint32_t verticesCount);
	bool geometryModelSetIndices(uint64_t geometryModelHandle, uint64_t sharedDataHandle, uint32_t indicesCount, VkIndexType indexType, uint32_t indexOffset, uint32_t indexRange);
	bool geometryModelSetTarget(uint64_t geometryModelHandle, uint64_t sharedDataHandle, const std::string& targetName);
	bool geometryModelSetCullMode(uint64_t geometryModelHandle, VkCullModeFlags cullMode);

	bool groupAddGeometryModel(uint64_t groupHandle, uint64_t geometryModelHandle);

	bool instanceSetWorldMatrix(uint64_t instanceHandle, const glm::mat4& worldMatrix);
	bool instanceSetGroup(uint64_t instanceHandle, uint64_t groupHandle);

	bool lightSetEnvironment(uint64_t lightHandle, const std::string& environment);

	bool worldAddInstance(uint64_t instanceHandle);
	bool worldSetLight(uint64_t lightHandle);
	bool worldSetCamera(uint64_t cameraHandle);

	// Finalization and setup not allowed anymore.

	bool sharedDataFinalize(uint64_t sharedDataHandle);

	bool textureFinalize(uint64_t textureHandle);

	bool materialFinalize(uint64_t materialHandle);

	bool geometryFinalize(uint64_t geometryHandle);

	bool geometryModelFinalize(uint64_t geometryModelHandle);

	bool groupFinalize(uint64_t groupHandle);

	bool instanceFinalize(uint64_t instanceHandle);

	bool lightFinalize(uint64_t lightHandle);

	bool cameraFinalize(uint64_t cameraHandle);

	bool worldFinalize();

	// Getters

	bool worldGetCamera(uint64_t& cameraHandle);

	// Update also after finalization.

	bool instanceUpdateWorldMatrix(uint64_t instanceHandle, const glm::mat4& worldMatrix);

	bool cameraUpdateProjectionMatrix(uint64_t cameraHandle, const glm::mat4& projectionMatrix);
	bool cameraUpdateViewMatrix(uint64_t cameraHandle, const glm::mat4& viewMatrix);

	// Delete and free data.

	bool sharedDataDelete(uint64_t sharedDataHandle);

	bool textureDelete(uint64_t textureHandle);

	bool materialDelete(uint64_t materialHandle);

	bool geometryDelete(uint64_t geometryHandle);

	bool geometryModelDelete(uint64_t geometryModelHandle);

	bool groupDelete(uint64_t groupHandle);

	bool instanceDelete(uint64_t instanceHandle);

	bool lightDelete(uint64_t lightHandle);

	bool cameraDelete(uint64_t cameraHandle);

	bool worldDelete();

	//

	void terminate();

	//
	// Rendering
	//

	void rasterize(VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode);

	void raytrace(VkCommandBuffer commandBuffer, uint32_t frameIndex);

};

#endif /* RENDER_RENDERMANAGER_H_ */
