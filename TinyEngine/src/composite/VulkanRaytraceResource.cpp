#include "../common/Logger.h"
#include "../vulkan/HelperVulkan.h"
#include "VulkanRaytraceResource.h"


bool VulkanRaytraceResource::createAccelerationStructureResource(VkPhysicalDevice physicalDevice, VkDevice device, AccelerationStructureResource& accelerationStructureResource, const AccelerationStructureResourceCreateInfo& accelerationStructureResourceCreateInfo)
{
	VkResult result = VK_SUCCESS;

	VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo = {};
	accelerationStructureCreateInfo.sType            = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	accelerationStructureCreateInfo.type             = accelerationStructureResourceCreateInfo.type;
	accelerationStructureCreateInfo.flags            = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	accelerationStructureCreateInfo.maxGeometryCount = accelerationStructureResourceCreateInfo.accelerationStructureCreateGeometryTypeInfos.size();
	accelerationStructureCreateInfo.pGeometryInfos   = accelerationStructureResourceCreateInfo.accelerationStructureCreateGeometryTypeInfos.data();

	result = vkCreateAccelerationStructureKHR(device, &accelerationStructureCreateInfo, nullptr, &accelerationStructureResource.accelerationStructure);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	VkMemoryRequirements2 memoryRequirements2 = {};
	memoryRequirements2.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;

	VkAccelerationStructureMemoryRequirementsInfoKHR accelerationStructureMemoryRequirementsInfo = {};

	accelerationStructureMemoryRequirementsInfo.sType                 = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_KHR;
	accelerationStructureMemoryRequirementsInfo.type                  = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_KHR;
	accelerationStructureMemoryRequirementsInfo.buildType             = VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR;
	accelerationStructureMemoryRequirementsInfo.accelerationStructure = accelerationStructureResource.accelerationStructure;

	vkGetAccelerationStructureMemoryRequirementsKHR(device, &accelerationStructureMemoryRequirementsInfo, &memoryRequirements2);

	VkMemoryRequirements memoryRequirements = memoryRequirements2.memoryRequirements;

	uint32_t memoryTypeIndex = 0;
	if (!HelperVulkan::findMemoryTypeIndex(memoryTypeIndex, physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
	{
		return false;
	}

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize  = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

	result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &accelerationStructureResource.deviceMemory);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	VkBindAccelerationStructureMemoryInfoKHR bindAccelerationStructureMemoryInfo = {};
	bindAccelerationStructureMemoryInfo.sType                 = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_KHR;
	bindAccelerationStructureMemoryInfo.accelerationStructure = accelerationStructureResource.accelerationStructure;
	bindAccelerationStructureMemoryInfo.memory                = accelerationStructureResource.deviceMemory;

	result = vkBindAccelerationStructureMemoryKHR(device, 1, &bindAccelerationStructureMemoryInfo);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	return true;
}

void VulkanRaytraceResource::destroyAccelerationStructureResource(VkDevice device, AccelerationStructureResource& accelerationStructureResource)
{
	if (accelerationStructureResource.deviceMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(device, accelerationStructureResource.deviceMemory, nullptr);
		accelerationStructureResource.deviceMemory = VK_NULL_HANDLE;
	}

	if (accelerationStructureResource.accelerationStructure != VK_NULL_HANDLE)
	{
		vkDestroyAccelerationStructureKHR(device, accelerationStructureResource.accelerationStructure, nullptr);
		accelerationStructureResource.accelerationStructure = VK_NULL_HANDLE;
	}
}

bool VulkanRaytraceResource::createScratchBuffer(VkPhysicalDevice physicalDevice, VkDevice device, ScratchBufferResource& scratchBufferResource, const ScratchBufferResourceCreateInfo& scratchBufferResourceCreateInfo)
{
	VkAccelerationStructureMemoryRequirementsInfoKHR accelerationStructureMemoryRequirementsInfo = {};
	accelerationStructureMemoryRequirementsInfo.sType                 = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_KHR;
	accelerationStructureMemoryRequirementsInfo.type                  = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_BUILD_SCRATCH_KHR;
	accelerationStructureMemoryRequirementsInfo.buildType             = VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR;
	accelerationStructureMemoryRequirementsInfo.accelerationStructure = scratchBufferResourceCreateInfo.accelerationStructure;

	VkMemoryRequirements2 memoryRequirements2 = {};
	memoryRequirements2.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;

	vkGetAccelerationStructureMemoryRequirementsKHR(device, &accelerationStructureMemoryRequirementsInfo, &memoryRequirements2);

	//

	VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo = {};
	memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	memoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

	BufferResourceCreateInfo bufferResourceCreateInfo = {};
	bufferResourceCreateInfo.size = memoryRequirements2.memoryRequirements.size;
	bufferResourceCreateInfo.pNext = &memoryAllocateFlagsInfo;
	bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	if (!VulkanResource::createBufferResource(physicalDevice, device, scratchBufferResource.bufferResource, bufferResourceCreateInfo))
	{
		return false;
	}

	return true;
}

void VulkanRaytraceResource::destroyScratchBuffer(VkDevice device, ScratchBufferResource& scratchBufferResource)
{
	VulkanResource::destroyBufferResource(device, scratchBufferResource.bufferResource);
}

void VulkanRaytraceResource::destroyLevelResource(VkDevice device, LevelResource& levelResource)
{
	VulkanRaytraceResource::destroyScratchBuffer(device, levelResource.scratchBufferResource);
	VulkanRaytraceResource::destroyAccelerationStructureResource(device, levelResource.accelerationStructureResource);
}

bool VulkanRaytraceResource::buildAccelerationStructure(VkDevice device, VkQueue queue, VkCommandPool commandPool, uint32_t infoCount, VkAccelerationStructureBuildGeometryInfoKHR* accelerationStructureBuildGeometryInfos, VkAccelerationStructureBuildOffsetInfoKHR** accelerationStructureBuildOffsetInfos, bool useHostCommand)
{
	VkResult result = VK_SUCCESS;

	//

	if (useHostCommand)
	{
		result = vkBuildAccelerationStructureKHR(device, infoCount, accelerationStructureBuildGeometryInfos, accelerationStructureBuildOffsetInfos);
		if (result != VK_SUCCESS)
		{
			Logger::print(TE_ERROR, __FILE__, __LINE__, result);

			return false;
		}
	}
	else
	{
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

		if (!HelperVulkan::beginOneTimeSubmitCommand(device, commandPool, commandBuffer))
		{
			return false;
		}

		vkCmdBuildAccelerationStructureKHR(commandBuffer, infoCount, accelerationStructureBuildGeometryInfos, accelerationStructureBuildOffsetInfos);

		if (!HelperVulkan::endOneTimeSubmitCommand(device, queue, commandPool, commandBuffer))
		{
			return false;
		}
	}

	return true;
}

bool VulkanRaytraceResource::createBottomLevelResource(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, BottomLevelResource& bottomLevelResource, const BottomLevelResourceCreateInfo& bottomLevelResourceCreateInfo)
{
	AccelerationStructureResourceCreateInfo accelerationStructureResourceCreateInfo = {};
	accelerationStructureResourceCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

	VkAccelerationStructureCreateGeometryTypeInfoKHR accelerationStructureCreateGeometryTypeInfo = {};
	accelerationStructureCreateGeometryTypeInfo.sType             = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR;
	accelerationStructureCreateGeometryTypeInfo.geometryType      = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	accelerationStructureCreateGeometryTypeInfo.maxPrimitiveCount = bottomLevelResourceCreateInfo.primitiveCount;
	accelerationStructureCreateGeometryTypeInfo.indexType         = bottomLevelResourceCreateInfo.indexType;
	accelerationStructureCreateGeometryTypeInfo.maxVertexCount    = bottomLevelResourceCreateInfo.vertexCount;
	accelerationStructureCreateGeometryTypeInfo.vertexFormat      = bottomLevelResourceCreateInfo.vertexFormat;
	accelerationStructureCreateGeometryTypeInfo.allowsTransforms  = VK_FALSE;

	accelerationStructureResourceCreateInfo.accelerationStructureCreateGeometryTypeInfos.push_back(accelerationStructureCreateGeometryTypeInfo);

	if (!VulkanRaytraceResource::createAccelerationStructureResource(physicalDevice, device, bottomLevelResource.levelResource.accelerationStructureResource, accelerationStructureResourceCreateInfo))
	{
		return false;
	}

	//

	ScratchBufferResourceCreateInfo scratchBufferResourceCreateInfo = {};
	scratchBufferResourceCreateInfo.accelerationStructure = bottomLevelResource.levelResource.accelerationStructureResource.accelerationStructure;

	if (!VulkanRaytraceResource::createScratchBuffer(physicalDevice, device, bottomLevelResource.levelResource.scratchBufferResource, scratchBufferResourceCreateInfo))
	{
		return false;
	}

	//

	VkBufferDeviceAddressInfo bufferDeviceAddressInfo = {};
	bufferDeviceAddressInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;

	bufferDeviceAddressInfo.buffer = bottomLevelResource.levelResource.scratchBufferResource.bufferResource.buffer;
	VkDeviceAddress scratchBufferAddress = vkGetBufferDeviceAddress(device, &bufferDeviceAddressInfo);

	VkAccelerationStructureGeometryKHR accelerationStructureGeometry = {};
	accelerationStructureGeometry.sType                                       = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	accelerationStructureGeometry.flags                                       = VK_GEOMETRY_OPAQUE_BIT_KHR;
	accelerationStructureGeometry.geometryType                                = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	accelerationStructureGeometry.geometry.triangles.sType                    = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
	accelerationStructureGeometry.geometry.triangles.vertexFormat             = accelerationStructureCreateGeometryTypeInfo.vertexFormat;
	accelerationStructureGeometry.geometry.triangles.vertexData.deviceAddress = bottomLevelResourceCreateInfo.vertexBufferAddress;
	accelerationStructureGeometry.geometry.triangles.vertexStride             = bottomLevelResourceCreateInfo.vertexStride;
	accelerationStructureGeometry.geometry.triangles.indexType                = accelerationStructureCreateGeometryTypeInfo.indexType;
	accelerationStructureGeometry.geometry.triangles.indexData.deviceAddress  = bottomLevelResourceCreateInfo.vertexIndexBufferAddress;

	std::vector<VkAccelerationStructureGeometryKHR*> accelerationStructureGeometries;
	accelerationStructureGeometries.push_back(&accelerationStructureGeometry);

	VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo = {};
	accelerationStructureBuildGeometryInfo.sType                     = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	accelerationStructureBuildGeometryInfo.type                      = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	accelerationStructureBuildGeometryInfo.flags                     = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	accelerationStructureBuildGeometryInfo.dstAccelerationStructure  = bottomLevelResource.levelResource.accelerationStructureResource.accelerationStructure;
	accelerationStructureBuildGeometryInfo.geometryCount             = accelerationStructureGeometries.size();
	accelerationStructureBuildGeometryInfo.ppGeometries              = accelerationStructureGeometries.data();
	accelerationStructureBuildGeometryInfo.scratchData.deviceAddress = scratchBufferAddress;

	//

	VkAccelerationStructureBuildOffsetInfoKHR accelerationStructureBuildOffsetInfo = {};
	accelerationStructureBuildOffsetInfo.primitiveCount  = bottomLevelResourceCreateInfo.primitiveCount;
	accelerationStructureBuildOffsetInfo.primitiveOffset = 0x0;
	accelerationStructureBuildOffsetInfo.firstVertex     = 0;
	accelerationStructureBuildOffsetInfo.transformOffset = 0x0;

	std::vector<VkAccelerationStructureBuildOffsetInfoKHR*> accelerationStructureBuildOffsetInfos;
	accelerationStructureBuildOffsetInfos.push_back(&accelerationStructureBuildOffsetInfo);

	//

	if (!VulkanRaytraceResource::buildAccelerationStructure(device, queue, commandPool, 1, &accelerationStructureBuildGeometryInfo, accelerationStructureBuildOffsetInfos.data(), bottomLevelResourceCreateInfo.useHostCommand))
	{
		return false;
	}

	return true;
}

void VulkanRaytraceResource::destroyBottomLevelResource(VkDevice device, BottomLevelResource& bottomLevelResource)
{
	VulkanRaytraceResource::destroyLevelResource(device, bottomLevelResource.levelResource);
}

bool VulkanRaytraceResource::createTopLevelResource(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, TopLevelResource& topLevelResource, const TopLevelResourceCreateInfo& topLevelResourceCreateInfo)
{
	AccelerationStructureResourceCreateInfo accelerationStructureResourceCreateInfo = {};
	accelerationStructureResourceCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;

	VkAccelerationStructureCreateGeometryTypeInfoKHR accelerationStructureCreateGeometryTypeInfo = {};
	accelerationStructureCreateGeometryTypeInfo.sType             = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR;
	accelerationStructureCreateGeometryTypeInfo.geometryType      = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	accelerationStructureCreateGeometryTypeInfo.maxPrimitiveCount = topLevelResourceCreateInfo.primitiveCount;
	accelerationStructureCreateGeometryTypeInfo.allowsTransforms  = VK_TRUE;

	accelerationStructureResourceCreateInfo.accelerationStructureCreateGeometryTypeInfos.push_back(accelerationStructureCreateGeometryTypeInfo);

	if (!VulkanRaytraceResource::createAccelerationStructureResource(physicalDevice, device, topLevelResource.levelResource.accelerationStructureResource, accelerationStructureResourceCreateInfo))
	{
		return false;
	}

	//

	ScratchBufferResourceCreateInfo scratchBufferResourceCreateInfo = {};
	scratchBufferResourceCreateInfo.accelerationStructure = topLevelResource.levelResource.accelerationStructureResource.accelerationStructure;

	if (!VulkanRaytraceResource::createScratchBuffer(physicalDevice, device, topLevelResource.levelResource.scratchBufferResource, scratchBufferResourceCreateInfo))
	{
		return false;
	}

	//

	VkBufferDeviceAddressInfo bufferDeviceAddressInfo = {};
	bufferDeviceAddressInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;

	bufferDeviceAddressInfo.buffer = topLevelResource.levelResource.scratchBufferResource.bufferResource.buffer;
	VkDeviceAddress scratchBufferAddress = vkGetBufferDeviceAddress(device, &bufferDeviceAddressInfo);

	VkAccelerationStructureGeometryKHR accelerationStructureGeometry = {};
	accelerationStructureGeometry.sType                                 = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	accelerationStructureGeometry.flags                                 = VK_GEOMETRY_OPAQUE_BIT_KHR;
	accelerationStructureGeometry.geometryType                          = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	accelerationStructureGeometry.geometry.instances.sType              = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	accelerationStructureGeometry.geometry.instances.arrayOfPointers    = VK_FALSE;
	accelerationStructureGeometry.geometry.instances.data.deviceAddress = topLevelResourceCreateInfo.deviceAddress;

	std::vector<VkAccelerationStructureGeometryKHR*> accelerationStructureGeometries;
	accelerationStructureGeometries.push_back(&accelerationStructureGeometry);

	VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo = {};
	accelerationStructureBuildGeometryInfo.sType                     = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	accelerationStructureBuildGeometryInfo.type                      = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	accelerationStructureBuildGeometryInfo.flags                     = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
	accelerationStructureBuildGeometryInfo.update                    = VK_FALSE;
	accelerationStructureBuildGeometryInfo.srcAccelerationStructure  = VK_NULL_HANDLE;
	accelerationStructureBuildGeometryInfo.dstAccelerationStructure  = topLevelResource.levelResource.accelerationStructureResource.accelerationStructure;
	accelerationStructureBuildGeometryInfo.geometryArrayOfPointers   = VK_FALSE;
	accelerationStructureBuildGeometryInfo.geometryCount             = accelerationStructureGeometries.size();
	accelerationStructureBuildGeometryInfo.ppGeometries              = accelerationStructureGeometries.data();
	accelerationStructureBuildGeometryInfo.scratchData.deviceAddress = scratchBufferAddress;

	VkAccelerationStructureBuildOffsetInfoKHR accelerationStructureBuildOffsetInfo = {};
	accelerationStructureBuildOffsetInfo.primitiveCount  = topLevelResourceCreateInfo.primitiveCount;
	accelerationStructureBuildOffsetInfo.primitiveOffset = 0x0;
	accelerationStructureBuildOffsetInfo.firstVertex     = 0;
	accelerationStructureBuildOffsetInfo.transformOffset = 0x0;

	std::vector<VkAccelerationStructureBuildOffsetInfoKHR*> accelerationStructureBuildOffsetInfos;
	accelerationStructureBuildOffsetInfos.push_back(&accelerationStructureBuildOffsetInfo);

	//

	if (!VulkanRaytraceResource::buildAccelerationStructure(device, queue, commandPool, 1, &accelerationStructureBuildGeometryInfo, accelerationStructureBuildOffsetInfos.data(), topLevelResourceCreateInfo.useHostCommand))
	{
		return false;
	}

	return true;
}

void VulkanRaytraceResource::destroyTopLevelResource(VkDevice device, TopLevelResource& topLevelResource)
{
	VulkanRaytraceResource::destroyLevelResource(device, topLevelResource.levelResource);
}

