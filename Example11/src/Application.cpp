#include "Application.h"

// Private

struct VertexData {
	glm::vec4 position;
};

struct UniformData {
	glm::mat4 inverseProjection;
	glm::mat4 inverseView;
};

bool Application::applicationInit()
{
	VkResult result = VK_SUCCESS;

	//
	// Indices and vertices for the bottom level acceleration structure.
	//

	std::vector<uint16_t> vertexIndices = {
	    0, 1, 2
	};

	VertexBufferResourceCreateInfo vertexIndexBufferResourceCreateInfo = {};

	vertexIndexBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(uint16_t) * vertexIndices.size();
	vertexIndexBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	vertexIndexBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	vertexIndexBufferResourceCreateInfo.data = vertexIndices.data();

	if (!VulkanResource::createVertexBufferResource(physicalDevice, device, queue, commandPool, vertexIndexBufferResource, vertexIndexBufferResourceCreateInfo))
	{
		return false;
	}

	//

	std::vector<VertexData> vertexData = {
		{glm::vec4( 0.0f, -0.5f, 0.0f, 1.0f)},
		{glm::vec4( 0.5f,  0.5f, 0.0f, 1.0f)},
		{glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f)}
	};

	VertexBufferResourceCreateInfo vertexBufferResourceCreateInfo = {};

	vertexBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(VertexData) * vertexData.size();
	vertexBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	vertexBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	vertexBufferResourceCreateInfo.data = vertexData.data();

	if (!VulkanResource::createVertexBufferResource(physicalDevice, device, queue, commandPool, vertexBufferResource, vertexBufferResourceCreateInfo))
	{
		return false;
	}

	VkBufferDeviceAddressInfo bufferDeviceAddressInfo = {};
	bufferDeviceAddressInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;

	bufferDeviceAddressInfo.buffer = vertexIndexBufferResource.bufferResource.buffer;
	VkDeviceAddress vertexIndexBufferAddress = vkGetBufferDeviceAddress(device, &bufferDeviceAddressInfo);

	bufferDeviceAddressInfo.buffer = vertexBufferResource.bufferResource.buffer;
	VkDeviceAddress vertexBufferAddress = vkGetBufferDeviceAddress(device, &bufferDeviceAddressInfo);

	//
	// Bottom level acceleration
	//

	BottomLevelResourceCreateInfo bottomLevelResourceCreateInfo = {};
	bottomLevelResourceCreateInfo.indexType = VK_INDEX_TYPE_UINT16;
	bottomLevelResourceCreateInfo.vertexIndexBufferAddress = vertexIndexBufferAddress;
	bottomLevelResourceCreateInfo.vertexCount = 3;
	bottomLevelResourceCreateInfo.vertexFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
	bottomLevelResourceCreateInfo.vertexBufferAddress = vertexBufferAddress;
	bottomLevelResourceCreateInfo.vertexStride = sizeof(VertexData);;
	bottomLevelResourceCreateInfo.primitiveCount = 1;
	bottomLevelResourceCreateInfo.useHostCommand = physicalDeviceRayTracingFeatures.rayTracingHostAccelerationStructureCommands;

	if (!VulkanRaytraceResource::createBottomLevelResource(physicalDevice, device, queue, commandPool, bottomLevelResource, bottomLevelResourceCreateInfo))
	{
		return false;
	}

	//
	// Top level.
	//

	//
	// Acceleration instance.
	//

	VkAccelerationStructureDeviceAddressInfoKHR accelerationStructureDeviceAddressInfo = {};
	accelerationStructureDeviceAddressInfo.sType                 = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	accelerationStructureDeviceAddressInfo.accelerationStructure = bottomLevelResource.levelResource.accelerationStructureResource.accelerationStructure;
	VkDeviceAddress bottomDeviceAddress = vkGetAccelerationStructureDeviceAddressKHR(device, &accelerationStructureDeviceAddressInfo);

	VkTransformMatrixKHR transformMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f
	};

	VkAccelerationStructureInstanceKHR accelerationStructureInstance = {};
	accelerationStructureInstance.transform                              = transformMatrix;
	accelerationStructureInstance.instanceCustomIndex                    = 0;
	accelerationStructureInstance.mask                                   = 0xFF;
	accelerationStructureInstance.instanceShaderBindingTableRecordOffset = 0;
	accelerationStructureInstance.flags                                  = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
	accelerationStructureInstance.accelerationStructureReference         = bottomDeviceAddress;

	BufferResourceCreateInfo bufferResourceCreateInfo = {};
	bufferResourceCreateInfo.size = sizeof(VkAccelerationStructureInstanceKHR);
	bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	if (!VulkanResource::createBufferResource(physicalDevice, device, accelerationStructureInstanceBuffer, bufferResourceCreateInfo))
	{
		return false;
	}

	if (!VulkanResource::copyHostToDevice(device, accelerationStructureInstanceBuffer, &accelerationStructureInstance, sizeof(VkAccelerationStructureInstanceKHR)))
	{
		return false;
	}

	bufferDeviceAddressInfo.buffer = accelerationStructureInstanceBuffer.buffer;
	VkDeviceAddress deviceAddress = vkGetBufferDeviceAddressKHR(device, &bufferDeviceAddressInfo);

	//
	// Top level acceleration
	//

	TopLevelResourceCreateInfo topLevelResourceCreateInfo = {};
	topLevelResourceCreateInfo.deviceAddress = deviceAddress;
	topLevelResourceCreateInfo.primitiveCount = 1;
	topLevelResourceCreateInfo.useHostCommand = physicalDeviceRayTracingFeatures.rayTracingHostAccelerationStructureCommands;

	if (!VulkanRaytraceResource::createTopLevelResource(physicalDevice, device, queue, commandPool, topLevelResource, topLevelResourceCreateInfo))
	{
		return false;
	}

	// Level structures done.

	//
	// Raytracing output buffer
	//

	ImageViewResourceCreateInfo imageViewResourceCreateInfo = {};
	imageViewResourceCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
	imageViewResourceCreateInfo.extent = {width, height, 1};
	imageViewResourceCreateInfo.mipLevels = 1;
	imageViewResourceCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageViewResourceCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
	imageViewResourceCreateInfo.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	if (!VulkanResource::createImageViewResource(physicalDevice, device, raytraceImageViewResource, imageViewResourceCreateInfo))
	{
		return false;
	}

	//
	// Uniform buffer for view and projection matrices.
	//

	UniformData uniformData = {};
	uniformData.inverseProjection = glm::inverse(Projection::perspective(60.0f, (float)width / (float)height, 0.1f, 100.0f));
	uniformData.inverseView = glm::inverse(glm::lookAt(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

	UniformBufferResourceCreateInfo uniformBufferResourceCreateInfo = {};

	uniformBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(UniformData);
	uniformBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	uniformBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	uniformBufferResourceCreateInfo.data = &uniformData;

	if (!VulkanResource::createUniformBufferResource(physicalDevice, device, uniformBufferResource, uniformBufferResourceCreateInfo))
	{
		return false;
	}

	//
	// Create required descriptor sets
	//

	uint32_t bindingCount = 3;
	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings(bindingCount);

	descriptorSetLayoutBindings[0].binding         = 0;
	descriptorSetLayoutBindings[0].descriptorType  = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	descriptorSetLayoutBindings[0].descriptorCount = 1;
	descriptorSetLayoutBindings[0].stageFlags      = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

	descriptorSetLayoutBindings[1].binding         = 1;
	descriptorSetLayoutBindings[1].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorSetLayoutBindings[1].descriptorCount = 1;
	descriptorSetLayoutBindings[1].stageFlags      = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

	descriptorSetLayoutBindings[2].binding         = 2;
	descriptorSetLayoutBindings[2].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorSetLayoutBindings[2].descriptorCount = 1;
	descriptorSetLayoutBindings[2].stageFlags      = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = bindingCount;
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

	result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	std::vector<VkDescriptorPoolSize> descriptorPoolSizes(bindingCount);
	descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	descriptorPoolSizes[0].descriptorCount = 1;
	descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorPoolSizes[1].descriptorCount = 1;
	descriptorPoolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSizes[2].descriptorCount = 1;

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.poolSizeCount = bindingCount;
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
	descriptorPoolCreateInfo.maxSets = 1;

	result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

	result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	VkWriteDescriptorSetAccelerationStructureKHR writeDescriptorSetAccelerationStructure = {};
	writeDescriptorSetAccelerationStructure.sType                      = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
	writeDescriptorSetAccelerationStructure.accelerationStructureCount = 1;
	writeDescriptorSetAccelerationStructure.pAccelerationStructures    = &topLevelResource.levelResource.accelerationStructureResource.accelerationStructure;

	VkDescriptorImageInfo descriptorImageInfo = {};
	descriptorImageInfo.sampler = VK_NULL_HANDLE;
	descriptorImageInfo.imageView = raytraceImageViewResource.imageView;
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkDescriptorBufferInfo descriptorBufferInfo = {};
	descriptorBufferInfo.buffer = uniformBufferResource.bufferResource.buffer;
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = sizeof(UniformData);

	std::vector<VkWriteDescriptorSet> writeDescriptorSets(bindingCount);

	writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSets[0].dstSet          = descriptorSet;
	writeDescriptorSets[0].dstBinding      = 0;
	writeDescriptorSets[0].descriptorType  = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	writeDescriptorSets[0].descriptorCount = 1;
	writeDescriptorSets[0].pNext           = &writeDescriptorSetAccelerationStructure;	// Chained

	writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSets[1].dstSet = descriptorSet;
	writeDescriptorSets[1].dstBinding = 1;
	writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	writeDescriptorSets[1].descriptorCount = 1;
	writeDescriptorSets[1].pImageInfo = &descriptorImageInfo;

	writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSets[2].dstSet = descriptorSet;
	writeDescriptorSets[2].dstBinding = 2;
	writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSets[2].descriptorCount = 1;
	writeDescriptorSets[2].pBufferInfo = &descriptorBufferInfo;

	vkUpdateDescriptorSets(device, bindingCount, writeDescriptorSets.data(), 0, nullptr);

	//
	// Creating ray tracing pipeline.
	//

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts    = &descriptorSetLayout;

	result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	//
	// Build the required raytracing shaders.
	//

	std::map<std::string, std::string> macros;


	std::string rayGenShaderSource = "";
	if (!FileIO::open(rayGenShaderSource, "../Resources/shaders/raygen.rgen"))
	{
		return false;
	}

	std::vector<uint32_t> rayGenShaderCode;
	if (!Compiler::buildShader(rayGenShaderCode, rayGenShaderSource, macros, shaderc_raygen_shader))
	{
		return false;
	}

	if (!VulkanResource::createShaderModule(rayGenShaderModule, device, rayGenShaderCode))
	{
		return false;
	}

	//

	std::string missShaderSource = "";
	if (!FileIO::open(missShaderSource, "../Resources/shaders/miss.rmiss"))
	{
		return false;
	}

	std::vector<uint32_t> missShaderCode;
	if (!Compiler::buildShader(missShaderCode, missShaderSource, macros, shaderc_miss_shader))
	{
		return false;
	}

	if (!VulkanResource::createShaderModule(missShaderModule, device, missShaderCode))
	{
		return false;
	}

	//

	std::string closestHitShaderSource = "";
	if (!FileIO::open(closestHitShaderSource, "../Resources/shaders/closesthit.rchit"))
	{
		return false;
	}

	std::vector<uint32_t> closestHitShaderCode;
	if (!Compiler::buildShader(closestHitShaderCode, closestHitShaderSource, macros, shaderc_closesthit_shader))
	{
		return false;
	}

	if (!VulkanResource::createShaderModule(closestHitShaderModule, device, closestHitShaderCode))
	{
		return false;
	}

	//
	//

	std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;
	pipelineShaderStageCreateInfos.resize(3);

	pipelineShaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfos[0].stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	pipelineShaderStageCreateInfos[0].module = rayGenShaderModule;
	pipelineShaderStageCreateInfos[0].pName = "main";

	pipelineShaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfos[1].stage = VK_SHADER_STAGE_MISS_BIT_KHR;
	pipelineShaderStageCreateInfos[1].module = missShaderModule;
	pipelineShaderStageCreateInfos[1].pName = "main";

	pipelineShaderStageCreateInfos[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfos[2].stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	pipelineShaderStageCreateInfos[2].module = closestHitShaderModule;
	pipelineShaderStageCreateInfos[2].pName = "main";

	//

	std::vector<VkRayTracingShaderGroupCreateInfoKHR> rayTracingShaderGroupCreateInfos = {};
	rayTracingShaderGroupCreateInfos.resize(3);

	rayTracingShaderGroupCreateInfos[0] = {};
	rayTracingShaderGroupCreateInfos[0].sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	rayTracingShaderGroupCreateInfos[0].type 			   = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	rayTracingShaderGroupCreateInfos[0].generalShader      = 0;
	rayTracingShaderGroupCreateInfos[0].closestHitShader   = VK_SHADER_UNUSED_KHR;
	rayTracingShaderGroupCreateInfos[0].anyHitShader       = VK_SHADER_UNUSED_KHR;
	rayTracingShaderGroupCreateInfos[0].intersectionShader = VK_SHADER_UNUSED_KHR;

	rayTracingShaderGroupCreateInfos[1] = {};
	rayTracingShaderGroupCreateInfos[1].sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	rayTracingShaderGroupCreateInfos[1].type 			   = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	rayTracingShaderGroupCreateInfos[1].generalShader      = 1;
	rayTracingShaderGroupCreateInfos[1].closestHitShader   = VK_SHADER_UNUSED_KHR;
	rayTracingShaderGroupCreateInfos[1].anyHitShader       = VK_SHADER_UNUSED_KHR;
	rayTracingShaderGroupCreateInfos[1].intersectionShader = VK_SHADER_UNUSED_KHR;

	rayTracingShaderGroupCreateInfos[2] = {};
	rayTracingShaderGroupCreateInfos[2].sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	rayTracingShaderGroupCreateInfos[2].type 			   = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
	rayTracingShaderGroupCreateInfos[2].generalShader      = VK_SHADER_UNUSED_KHR;
	rayTracingShaderGroupCreateInfos[2].closestHitShader   = 2;
	rayTracingShaderGroupCreateInfos[2].anyHitShader       = VK_SHADER_UNUSED_KHR;
	rayTracingShaderGroupCreateInfos[2].intersectionShader = VK_SHADER_UNUSED_KHR;

	VkRayTracingPipelineCreateInfoKHR rayTracingPipelineCreateInfo = {};
	rayTracingPipelineCreateInfo.sType             = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
	rayTracingPipelineCreateInfo.stageCount        = pipelineShaderStageCreateInfos.size();
	rayTracingPipelineCreateInfo.pStages           = pipelineShaderStageCreateInfos.data();
	rayTracingPipelineCreateInfo.groupCount        = rayTracingShaderGroupCreateInfos.size();
	rayTracingPipelineCreateInfo.pGroups           = rayTracingShaderGroupCreateInfos.data();
	rayTracingPipelineCreateInfo.maxRecursionDepth = 1;
	rayTracingPipelineCreateInfo.layout            = pipelineLayout;
	rayTracingPipelineCreateInfo.libraries.sType   = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR;

	result = vkCreateRayTracingPipelinesKHR(device, VK_NULL_HANDLE, 1, &rayTracingPipelineCreateInfo, nullptr, &pipeline);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	//
	// Build up shader binding table.
	//

	bufferResourceCreateInfo = {};
	bufferResourceCreateInfo.size = physicalDeviceRayTracingProperties.shaderGroupHandleSize * rayTracingShaderGroupCreateInfos.size();
	bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR;
	bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	if (!VulkanResource::createBufferResource(physicalDevice, device, shaderBindingBufferResource, bufferResourceCreateInfo))
	{
		return false;
	}

	std::vector<uint8_t> rayTracingShaderGroupHandles(bufferResourceCreateInfo.size);

	result = vkGetRayTracingShaderGroupHandlesKHR(device, pipeline, 0, rayTracingShaderGroupCreateInfos.size(), rayTracingShaderGroupHandles.size(), rayTracingShaderGroupHandles.data());
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	if (!VulkanResource::copyHostToDevice(device, shaderBindingBufferResource, rayTracingShaderGroupHandles.data(), rayTracingShaderGroupHandles.size()))
	{
		return false;
	}

	return true;
}

bool Application::applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime)
{
	VkImageMemoryBarrier imageMemoryBarrier[2] = {};

	imageMemoryBarrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier[0].subresourceRange.levelCount = 1;
	imageMemoryBarrier[0].subresourceRange.layerCount = 1;
	imageMemoryBarrier[0].image = raytraceImageViewResource.image;
	//
	imageMemoryBarrier[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier[1].subresourceRange.levelCount = 1;
	imageMemoryBarrier[1].subresourceRange.layerCount = 1;
	imageMemoryBarrier[1].image = swapchainImages[frameIndex];

	// Prepare to raytrace into image.

	imageMemoryBarrier[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	imageMemoryBarrier[0].dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
	imageMemoryBarrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageMemoryBarrier[0].newLayout = VK_IMAGE_LAYOUT_GENERAL;

	vkCmdPipelineBarrier(commandBuffers[frameIndex], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, imageMemoryBarrier);

	//
	// Raytracing part.
	//

	VkStridedBufferRegionKHR rayGenStridedBufferRegion = {};
	rayGenStridedBufferRegion.buffer = shaderBindingBufferResource.buffer;
	rayGenStridedBufferRegion.offset = physicalDeviceRayTracingProperties.shaderGroupHandleSize * 0;
	rayGenStridedBufferRegion.size   = physicalDeviceRayTracingProperties.shaderGroupHandleSize;

	VkStridedBufferRegionKHR missStridedBufferRegion = {};
	missStridedBufferRegion.buffer = shaderBindingBufferResource.buffer;
	missStridedBufferRegion.offset = physicalDeviceRayTracingProperties.shaderGroupHandleSize * 1;
	missStridedBufferRegion.size   = physicalDeviceRayTracingProperties.shaderGroupHandleSize;

	VkStridedBufferRegionKHR closestHitStridedBufferRegion = {};
	closestHitStridedBufferRegion.buffer = shaderBindingBufferResource.buffer;
	closestHitStridedBufferRegion.offset = physicalDeviceRayTracingProperties.shaderGroupHandleSize * 2;
	closestHitStridedBufferRegion.size   = physicalDeviceRayTracingProperties.shaderGroupHandleSize;

	VkStridedBufferRegionKHR callableStridedBufferRegion = {};

	vkCmdBindPipeline(commandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, pipeline);
	vkCmdBindDescriptorSets(commandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, pipelineLayout, 0, 1, &descriptorSet, 0, 0);

	vkCmdTraceRaysKHR(commandBuffers[frameIndex], &rayGenStridedBufferRegion, &missStridedBufferRegion, &closestHitStridedBufferRegion, &callableStridedBufferRegion, width, height, 1);

	//
	// Prepare to to copy raytraced image.
	//

	imageMemoryBarrier[0].srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
	imageMemoryBarrier[0].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	imageMemoryBarrier[0].oldLayout = VK_IMAGE_LAYOUT_GENERAL;
	imageMemoryBarrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

	imageMemoryBarrier[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	imageMemoryBarrier[1].dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
	imageMemoryBarrier[1].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	imageMemoryBarrier[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	vkCmdPipelineBarrier(commandBuffers[frameIndex], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 2, imageMemoryBarrier);

	VkImageCopy imageCopy = {};
	imageCopy.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
	imageCopy.srcOffset      = {0, 0, 0};
	imageCopy.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
	imageCopy.dstOffset      = {0, 0, 0};
	imageCopy.extent         = {width, height, 1};
	vkCmdCopyImage(commandBuffers[frameIndex], raytraceImageViewResource.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapchainImages[frameIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

	// Switch back to old layout.s

	imageMemoryBarrier[0].srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
	imageMemoryBarrier[0].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	imageMemoryBarrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	imageMemoryBarrier[0].newLayout = VK_IMAGE_LAYOUT_GENERAL;

	imageMemoryBarrier[1].srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
	imageMemoryBarrier[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	imageMemoryBarrier[1].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageMemoryBarrier[1].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	vkCmdPipelineBarrier(commandBuffers[frameIndex], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, NULL, 0, NULL, 2, imageMemoryBarrier);

	return true;
}

void Application::applicationTerminate()
{
	VulkanResource::destroyBufferResource(device, shaderBindingBufferResource);

	if (pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(device, pipeline, nullptr);
		pipeline = VK_NULL_HANDLE;
	}

	if (rayGenShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, rayGenShaderModule, nullptr);
		rayGenShaderModule = VK_NULL_HANDLE;
	}

	if (missShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, missShaderModule, nullptr);
		missShaderModule = VK_NULL_HANDLE;
	}

	if (closestHitShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, closestHitShaderModule, nullptr);
		closestHitShaderModule = VK_NULL_HANDLE;
	}

	if (pipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		pipelineLayout = VK_NULL_HANDLE;
	}

	descriptorSet = VK_NULL_HANDLE;

	if (descriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		descriptorPool = VK_NULL_HANDLE;
	}

	if (descriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		descriptorSetLayout = VK_NULL_HANDLE;
	}

	VulkanResource::destroyUniformBufferResource(device, uniformBufferResource);

	VulkanResource::destroyImageViewResource(device, raytraceImageViewResource);

	//

	VulkanRaytraceResource::destroyTopLevelResource(device, topLevelResource);

	VulkanResource::destroyBufferResource(device, accelerationStructureInstanceBuffer);

	//

	VulkanRaytraceResource::destroyBottomLevelResource(device, bottomLevelResource);

	VulkanResource::destroyVertexBufferResource(device, vertexBufferResource);
	VulkanResource::destroyVertexBufferResource(device, vertexIndexBufferResource);
}

// Public

Application::Application(const char* title) :
	TinyEngine(title)
{
}

Application::~Application()
{
}
