#ifndef COMPOSITE_HELPERRAYTRACE_H_
#define COMPOSITE_HELPERRAYTRACE_H_

#include <vector>

#include "HelperVulkanResource.h"

#define VK_ENABLE_BETA_EXTENSIONS
#include "volk.h"

struct AccelerationStructureResourceCreateInfo {
	VkAccelerationStructureTypeKHR type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	std::vector<VkAccelerationStructureCreateGeometryTypeInfoKHR> accelerationStructureCreateGeometryTypeInfos;
};

struct AccelerationStructureResource {
	VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE;
	VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
};


struct ScratchBufferResourceCreateInfo {
	VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE;
};

struct ScratchBufferResource {
	BufferResource bufferResource = {};
};

struct LevelResource {
	AccelerationStructureResource accelerationStructureResource = {};
	ScratchBufferResource scratchBufferResource = {};
};


struct BottomLevelResourceCreateInfo {
	VkIndexType indexType = VK_INDEX_TYPE_UINT16;
	VkDeviceAddress vertexIndexBufferAddress = 0;

	uint32_t vertexCount = 0;
	VkFormat vertexFormat = VK_FORMAT_UNDEFINED;
	VkDeviceAddress vertexBufferAddress = 0;
	VkDeviceSize vertexStride = 0;

	uint32_t primitiveCount = 0;

	bool useHostCommand = false;
};

struct BottomLevelResource {
	LevelResource levelResource = {};
};

struct TopLevelResourceCreateInfo {
	VkDeviceAddress deviceAddress;
	uint32_t primitiveCount = 0;

	bool useHostCommand = false;
};

struct TopLevelResource {
	LevelResource levelResource = {};
};

class HelperRaytrace
{
public:

	static bool createAccelerationStructureResource(VkPhysicalDevice physicalDevice, VkDevice device, AccelerationStructureResource& accelerationStructureResource, const AccelerationStructureResourceCreateInfo& accelerationStructureResourceCreateInfo);

	static void destroyAccelerationStructureResource(VkDevice device, AccelerationStructureResource& accelerationStructureResource);

	static bool createScratchBuffer(VkPhysicalDevice physicalDevice, VkDevice device, ScratchBufferResource& scratchBufferResource, const ScratchBufferResourceCreateInfo& scratchBufferResourceCreateInfo);

	static void destroyScratchBuffer(VkDevice device, ScratchBufferResource& scratchBufferResource);

	static void destroyLevelResource(VkDevice device, LevelResource& levelResource);

	static bool buildAccelerationStructure(VkDevice device, VkQueue queue, VkCommandPool commandPool, uint32_t infoCount, VkAccelerationStructureBuildGeometryInfoKHR* accelerationStructureBuildGeometryInfos, VkAccelerationStructureBuildOffsetInfoKHR** accelerationStructureBuildOffsetInfos, bool useHostCommand);

	static bool createBottomLevelResource(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, BottomLevelResource& bottomLevelResource, const BottomLevelResourceCreateInfo& bottomLevelResourceCreateInfo);

	static void destroyBottomLevelResource(VkDevice device, BottomLevelResource& bottomLevelResource);

	static bool createTopLevelResource(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, TopLevelResource& topLevelResource, const TopLevelResourceCreateInfo& topLevelResourceCreateInfo);

	static void destroyTopLevelResource(VkDevice device, TopLevelResource& topLevelResource);

};

#endif /* COMPOSITE_HELPERRAYTRACE_H_ */
