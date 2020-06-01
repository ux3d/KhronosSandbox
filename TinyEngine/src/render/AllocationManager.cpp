#include "AllocationManager.h"

#include "../shader/Shader.h"

AllocationManager::AllocationManager()
{
}

AllocationManager::~AllocationManager()
{
}

uint64_t AllocationManager::getBufferHandle(const Accessor& accessor)
{
	uint64_t sharedDataHandle = 0;

	if (accessor.aliasedBuffer.byteLength > 0)
	{
		sharedDataHandle = (uint64_t)&accessor.aliasedBufferView;

		return sharedDataHandle;
	}

	if (accessor.sparse.count >= 1)
	{
		sharedDataHandle = (uint64_t)&accessor.sparse.bufferView;

		return sharedDataHandle;
	}

	if (accessor.pBufferView == nullptr)
	{
		return sharedDataHandle;
	}

	sharedDataHandle = (uint64_t)accessor.pBufferView;

	return sharedDataHandle;
}

ResourceManager& AllocationManager::getResourceManager()
{
	return resourceManager;
}

bool AllocationManager::createSharedDataResource(const BufferView& bufferView, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, bool useRaytrace)
{
	uint64_t sharedDataHandle = (uint64_t)&bufferView;

	VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	if (bufferView.target == 34963) // ELEMENT_ARRAY_BUFFER
	{
		usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	}
	if (useRaytrace)
	{
		usage |= (VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	}
	resourceManager.sharedDataSetUsage(sharedDataHandle, usage);

	resourceManager.sharedDataSetData(sharedDataHandle, bufferView.byteLength, HelperAccess::accessData(bufferView));

	if (!resourceManager.sharedDataResourceFinalize(sharedDataHandle, physicalDevice, device, queue, commandPool))
	{
		return false;
	}

	return true;
}

bool AllocationManager::finalizePrimitive(const Primitive& primitive, const GLTF& glTF, std::map<std::string, std::string>& macros, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, uint32_t width, uint32_t height, VkRenderPass renderPass, VkSampleCountFlagBits samples, const VkDescriptorSetLayout* pSetLayouts, VkCullModeFlags cullMode, bool useRaytrace)
{
	GeometryModelResource* geometryModelResource = resourceManager.getGeometryModelResource((uint64_t)&primitive);

	if (!geometryModelResource)
	{
		return false;
	}

	GeometryResource* geometryResource = resourceManager.getGeometryResource(geometryModelResource->geometryHandle);

	//
	// Load the shader code.
	//

	std::string vertexShaderSource = "";
	if (!FileIO::open(vertexShaderSource, "../Resources/shaders/gltf.vert"))
	{
		return false;
	}

	std::string fragmentShaderSource = "";
	if (!FileIO::open(fragmentShaderSource, "../Resources/shaders/gltf.frag"))
	{
		return false;
	}

	//

	std::vector<uint32_t> vertexShaderCode;
	if (!Compiler::buildShader(vertexShaderCode, vertexShaderSource, macros, shaderc_vertex_shader))
	{
		return false;
	}

	std::vector<uint32_t> fragmentShaderCode;
	if (!Compiler::buildShader(fragmentShaderCode, fragmentShaderSource, macros, shaderc_fragment_shader))
	{
		return false;
	}

	if (!VulkanResource::createShaderModule(geometryModelResource->vertexShaderModule, device, vertexShaderCode))
	{
		return false;
	}

	if (!VulkanResource::createShaderModule(geometryModelResource->fragmentShaderModule, device, fragmentShaderCode))
	{
		return false;
	}

	//

	if (primitive.targetsCount > 0)
	{
		StorageBufferResourceCreateInfo storageBufferResourceCreateInfo = {};
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		if (primitive.targetPositionData.size() > 0)
		{
			storageBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(glm::vec3) * primitive.targetPositionData.size();
			storageBufferResourceCreateInfo.data = primitive.targetPositionData.data();

			if (!VulkanResource::createStorageBufferResource(physicalDevice, device, queue, commandPool, geometryModelResource->targetPosition, storageBufferResourceCreateInfo))
			{
				return false;
			}
		}

		if (primitive.targetNormalData.size() > 0)
		{
			storageBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(glm::vec3) * primitive.targetNormalData.size();
			storageBufferResourceCreateInfo.data = primitive.targetNormalData.data();

			if (!VulkanResource::createStorageBufferResource(physicalDevice, device, queue, commandPool, geometryModelResource->targetNormal, storageBufferResourceCreateInfo))
			{
				return false;
			}
		}

		if (primitive.targetTangentData.size() > 0)
		{
			storageBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(glm::vec3) * primitive.targetTangentData.size();
			storageBufferResourceCreateInfo.data = primitive.targetTangentData.data();

			if (!VulkanResource::createStorageBufferResource(physicalDevice, device, queue, commandPool, geometryModelResource->targetTangent, storageBufferResourceCreateInfo))
			{
				return false;
			}
		}
	}

	//

	VkResult result = VK_SUCCESS;

	VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[2] = {};

	pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	pipelineShaderStageCreateInfo[0].module = geometryModelResource->vertexShaderModule;
	pipelineShaderStageCreateInfo[0].pName = "main";

	pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	pipelineShaderStageCreateInfo[1].module = geometryModelResource->fragmentShaderModule;
	pipelineShaderStageCreateInfo[1].pName = "main";

	//

	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = primitive.attributesCount;
	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = geometryResource->vertexInputBindingDescriptions.data();
	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = primitive.attributesCount;
	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = geometryResource->vertexInputAttributeDescriptions.data();

	//

	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
	pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	//

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)width;
	viewport.height = (float)height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = {width, height};

	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
	pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipelineViewportStateCreateInfo.viewportCount = 1;
	pipelineViewportStateCreateInfo.pViewports = &viewport;
	pipelineViewportStateCreateInfo.scissorCount = 1;
	pipelineViewportStateCreateInfo.pScissors = &scissor;

	//

	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
	pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineRasterizationStateCreateInfo.cullMode = cullMode;
	pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

	//

	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
	pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineMultisampleStateCreateInfo.rasterizationSamples = samples;
	pipelineMultisampleStateCreateInfo.minSampleShading = 1.0f;

	//

	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
	pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;

	//

	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {};
	pipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
	pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
	pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipelineColorBlendStateCreateInfo.attachmentCount = 1;
	pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;

	//

	VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ModelViewProjectionUniformPushConstant);

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = (*pSetLayouts != VK_NULL_HANDLE) ? 1 : 0;
	pipelineLayoutCreateInfo.pSetLayouts = (*pSetLayouts != VK_NULL_HANDLE) ? pSetLayouts : nullptr;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

	result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &geometryModelResource->pipelineLayout);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	//

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.stageCount = 2;
	graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo;
	graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
	graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
	graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
	graphicsPipelineCreateInfo.layout = geometryModelResource->pipelineLayout;
	graphicsPipelineCreateInfo.renderPass = renderPass;

	result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &geometryModelResource->graphicsPipeline);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	//

	VkIndexType indexType = VK_INDEX_TYPE_NONE_KHR;
	if (primitive.indices >= 0)
	{
		indexType = VK_INDEX_TYPE_UINT8_EXT;
		if (glTF.accessors[primitive.indices].componentTypeSize == 2)
		{
			indexType = VK_INDEX_TYPE_UINT16;
		}
		else if (glTF.accessors[primitive.indices].componentTypeSize == 4)
		{
			indexType = VK_INDEX_TYPE_UINT32;
		}

		geometryModelResource->indexType = indexType;
		geometryModelResource->indexBuffer = resourceManager.getBuffer(getBufferHandle(glTF.accessors[primitive.indices]));
		geometryModelResource->indexOffset = HelperAccess::getOffset(glTF.accessors[primitive.indices]);

		geometryModelResource->count = glTF.accessors[primitive.indices].count;
	}
	else
	{
		geometryModelResource->count = glTF.accessors[primitive.position].count;
	}

	if (useRaytrace)
	{
		//
		// Bottom level.
		//

		VkBufferDeviceAddressInfo bufferDeviceAddressInfo = {};
		bufferDeviceAddressInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;

		bufferDeviceAddressInfo.buffer = resourceManager.getBuffer(getBufferHandle(glTF.accessors[primitive.position]));
		VkDeviceAddress vertexBufferAddress = vkGetBufferDeviceAddress(device, &bufferDeviceAddressInfo);

		uint32_t vertexCount = glTF.accessors[primitive.position].count;
		uint32_t primitiveCount = vertexCount / 3;

		VkDeviceAddress vertexIndexBufferAddress = 0;
		if (primitive.indices >= 0)
		{
			bufferDeviceAddressInfo.buffer = resourceManager.getBuffer(getBufferHandle(glTF.accessors[primitive.indices]));

			vertexIndexBufferAddress = vkGetBufferDeviceAddress(device, &bufferDeviceAddressInfo);

			primitiveCount = glTF.accessors[primitive.indices].count / 3;
		}

		//
		// Bottom level acceleration
		//

		VkFormat positionFormat = VK_FORMAT_UNDEFINED;

		if (!HelperVulkan::getFormat(positionFormat, glTF.accessors[primitive.position].componentTypeSize, glTF.accessors[primitive.position].componentTypeSigned, glTF.accessors[primitive.position].componentTypeInteger, glTF.accessors[primitive.position].typeCount, glTF.accessors[primitive.position].normalized))
		{
			return false;
		}

		BottomLevelResourceCreateInfo bottomLevelResourceCreateInfo = {};
		bottomLevelResourceCreateInfo.indexType = indexType;
		bottomLevelResourceCreateInfo.vertexIndexBufferAddress = vertexIndexBufferAddress;
		bottomLevelResourceCreateInfo.vertexCount = vertexCount;
		bottomLevelResourceCreateInfo.vertexFormat = positionFormat;
		bottomLevelResourceCreateInfo.vertexBufferAddress = vertexBufferAddress;
		bottomLevelResourceCreateInfo.vertexStride = glTF.accessors[primitive.position].typeCount * glTF.accessors[primitive.position].componentTypeSize;;
		bottomLevelResourceCreateInfo.primitiveCount = primitiveCount;
		bottomLevelResourceCreateInfo.useHostCommand = false;

		if (!VulkanRaytraceResource::createBottomLevelResource(physicalDevice, device, queue, commandPool, geometryModelResource->bottomLevelResource, bottomLevelResourceCreateInfo))
		{
			return false;
		}
	}

	return true;
}

bool AllocationManager::finalizeWorld(const GLTF& glTF, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImageView imageView, bool useRaytrace)
{
	WorldResource* worldResource = resourceManager.getWorldResource();

	if (!worldResource)
	{
		return false;
	}

	//

	if (useRaytrace)
	{
		TopLevelResourceCreateInfo topLevelResourceCreateInfo = {};

		uint32_t primitiveInstanceID = 0;
		int32_t normalInstanceID = 0;
		int32_t tangentInstanceID = 0;
		int32_t texCoord0InstanceID = 0;

		std::vector<VkDescriptorBufferInfo> descriptorBufferInfoIndices;
		std::vector<VkDescriptorBufferInfo> descriptorBufferInfoPosition;
		std::vector<VkDescriptorBufferInfo> descriptorBufferInfoNormal;
		std::vector<VkDescriptorBufferInfo> descriptorBufferInfoTangent;
		std::vector<VkDescriptorBufferInfo> descriptorBufferInfoTexCoord0;

		for (const Node& node : glTF.nodes)
		{
			if (node.mesh >= 0)
			{
				VkAccelerationStructureDeviceAddressInfoKHR accelerationStructureDeviceAddressInfo = {};
				accelerationStructureDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;

				VkTransformMatrixKHR transformMatrix = {
					node.worldMatrix[0][0], node.worldMatrix[1][0], node.worldMatrix[2][0], node.worldMatrix[3][0],
					node.worldMatrix[0][1], node.worldMatrix[1][1], node.worldMatrix[2][1], node.worldMatrix[3][1],
					node.worldMatrix[0][2], node.worldMatrix[1][2], node.worldMatrix[2][2], node.worldMatrix[3][2]
				};

				for (const Primitive& currentPrimitive : glTF.meshes[node.mesh].primitives)
				{
					GeometryModelResource* geometryModelResource = resourceManager.getGeometryModelResource((uint64_t)&currentPrimitive);
					GeometryResource* geometryResource = resourceManager.getGeometryResource((uint64_t)&currentPrimitive);

					//

					accelerationStructureDeviceAddressInfo.accelerationStructure = geometryModelResource->bottomLevelResource.levelResource.accelerationStructureResource.accelerationStructure;
					VkDeviceAddress bottomDeviceAddress = vkGetAccelerationStructureDeviceAddressKHR(device, &accelerationStructureDeviceAddressInfo);

					VkAccelerationStructureInstanceKHR accelerationStructureInstance = {};
					accelerationStructureInstance.transform                              = transformMatrix;
					accelerationStructureInstance.instanceCustomIndex                    = primitiveInstanceID;
					accelerationStructureInstance.mask                                   = 0xFF;
					accelerationStructureInstance.instanceShaderBindingTableRecordOffset = 0;
					accelerationStructureInstance.flags                                  = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
					accelerationStructureInstance.accelerationStructureReference         = bottomDeviceAddress;

					worldResource->accelerationStructureInstances.push_back(accelerationStructureInstance);

					RaytracePrimitiveUniformBuffer primitiveInformation = {};
					primitiveInformation.materialIndex = currentPrimitive.material;
					primitiveInformation.componentTypeSize = glTF.accessors[currentPrimitive.indices].componentTypeSize;
					primitiveInformation.worldMatrix = node.worldMatrix;

					if (geometryResource->normalAttributeIndex >= 0)
					{
						primitiveInformation.normalInstanceID = normalInstanceID;

						normalInstanceID++;
					}
					if (geometryResource->tangentAttributeIndex >= 0)
					{
						primitiveInformation.tangentInstanceID = tangentInstanceID;

						tangentInstanceID++;
					}
					if (geometryResource->texCoord0AttributeIndex >= 0)
					{
						primitiveInformation.texCoord0InstanceID = texCoord0InstanceID;

						texCoord0InstanceID++;
					}

					worldResource->instanceResources.push_back(primitiveInformation);

					primitiveInstanceID++;

					//
					// Gather descriptor buffer info.
					//

					if (currentPrimitive.indices >= 0)
					{
						VkDescriptorBufferInfo currentDescriptorBufferInfo = {};

						currentDescriptorBufferInfo.buffer = resourceManager.getBuffer(getBufferHandle(glTF.accessors[currentPrimitive.indices]));
						currentDescriptorBufferInfo.offset = HelperAccess::getOffset(glTF.accessors[currentPrimitive.indices]);
						currentDescriptorBufferInfo.range = HelperAccess::getRange(glTF.accessors[currentPrimitive.indices]);

						descriptorBufferInfoIndices.push_back(currentDescriptorBufferInfo);
					}

					if (geometryResource->positionAttributeIndex >= 0)
					{
						VkDescriptorBufferInfo currentDescriptorBufferInfo = {};

						currentDescriptorBufferInfo.buffer = resourceManager.getBuffer(getBufferHandle(glTF.accessors[currentPrimitive.position]));
						currentDescriptorBufferInfo.offset = HelperAccess::getOffset(glTF.accessors[currentPrimitive.position]);
						currentDescriptorBufferInfo.range =  HelperAccess::getRange(glTF.accessors[currentPrimitive.position]);

						descriptorBufferInfoPosition.push_back(currentDescriptorBufferInfo);
					}

					if (geometryResource->normalAttributeIndex >= 0)
					{
						VkDescriptorBufferInfo currentDescriptorBufferInfo = {};

						currentDescriptorBufferInfo.buffer = resourceManager.getBuffer(getBufferHandle(glTF.accessors[currentPrimitive.normal]));
						currentDescriptorBufferInfo.offset = HelperAccess::getOffset(glTF.accessors[currentPrimitive.normal]);
						currentDescriptorBufferInfo.range =  HelperAccess::getRange(glTF.accessors[currentPrimitive.normal]);

						descriptorBufferInfoNormal.push_back(currentDescriptorBufferInfo);
					}

					if (geometryResource->tangentAttributeIndex >= 0)
					{
						VkDescriptorBufferInfo currentDescriptorBufferInfo = {};

						currentDescriptorBufferInfo.buffer = resourceManager.getBuffer(getBufferHandle(glTF.accessors[currentPrimitive.tangent]));
						currentDescriptorBufferInfo.offset = HelperAccess::getOffset(glTF.accessors[currentPrimitive.tangent]);
						currentDescriptorBufferInfo.range =  HelperAccess::getRange(glTF.accessors[currentPrimitive.tangent]);

						descriptorBufferInfoTangent.push_back(currentDescriptorBufferInfo);
					}

					if (geometryResource->texCoord0AttributeIndex >= 0)
					{
						VkDescriptorBufferInfo currentDescriptorBufferInfo = {};

						currentDescriptorBufferInfo.buffer = resourceManager.getBuffer(getBufferHandle(glTF.accessors[currentPrimitive.texCoord0]));
						currentDescriptorBufferInfo.offset = HelperAccess::getOffset(glTF.accessors[currentPrimitive.texCoord0]);
						currentDescriptorBufferInfo.range =  HelperAccess::getRange(glTF.accessors[currentPrimitive.texCoord0]);

						descriptorBufferInfoTexCoord0.push_back(currentDescriptorBufferInfo);
					}
				}
			}
		}
		BufferResourceCreateInfo bufferResourceCreateInfo = {};
		bufferResourceCreateInfo.size = sizeof(VkAccelerationStructureInstanceKHR) * worldResource->accelerationStructureInstances.size();
		bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		if (!VulkanResource::createBufferResource(physicalDevice, device, worldResource->accelerationStructureInstanceBuffer, bufferResourceCreateInfo))
		{
			return false;
		}

		if (!VulkanResource::copyHostToDevice(device, worldResource->accelerationStructureInstanceBuffer, worldResource->accelerationStructureInstances.data(), sizeof(VkAccelerationStructureInstanceKHR) * worldResource->accelerationStructureInstances.size()))
		{
			return false;
		}

		VkBufferDeviceAddressInfo bufferDeviceAddressInfo = {};
		bufferDeviceAddressInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;

		bufferDeviceAddressInfo.buffer = worldResource->accelerationStructureInstanceBuffer.buffer;
		VkDeviceAddress deviceAddress = vkGetBufferDeviceAddressKHR(device, &bufferDeviceAddressInfo);

		//
		// Top level acceleration
		//

		topLevelResourceCreateInfo.deviceAddress = deviceAddress;
		topLevelResourceCreateInfo.primitiveCount = static_cast<uint32_t>(worldResource->accelerationStructureInstances.size());

		topLevelResourceCreateInfo.useHostCommand = false;

		if (!VulkanRaytraceResource::createTopLevelResource(physicalDevice, device, queue, commandPool, worldResource->topLevelResource, topLevelResourceCreateInfo))
		{
			return false;
		}

		//
		// Gather per instance resources.
		//

		StorageBufferResourceCreateInfo storageBufferResourceCreateInfo = {};
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(RaytracePrimitiveUniformBuffer) * worldResource->instanceResources.size();
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		storageBufferResourceCreateInfo.data = worldResource->instanceResources.data();

		if (!VulkanResource::createStorageBufferResource(physicalDevice, device, queue, commandPool, worldResource->instanceResourcesStorageBufferResource, storageBufferResourceCreateInfo))
		{
			return false;
		}

		//
		// Gather material resources.
		//

		std::vector<RaytraceMaterialUniformBuffer> materialBuffers;
		for (const Material& currentMatrial : glTF.materials)
		{
			RaytraceMaterialUniformBuffer uniformBufferRaytrace = {};

			uniformBufferRaytrace.materialUniformBuffer.baseColorFactor = currentMatrial.pbrMetallicRoughness.baseColorFactor;
			uniformBufferRaytrace.materialUniformBuffer.metallicFactor = currentMatrial.pbrMetallicRoughness.metallicFactor;
			uniformBufferRaytrace.materialUniformBuffer.roughnessFactor = currentMatrial.pbrMetallicRoughness.roughnessFactor;
			uniformBufferRaytrace.materialUniformBuffer.normalScale = currentMatrial.normalTexture.scale;
			uniformBufferRaytrace.materialUniformBuffer.occlusionStrength = currentMatrial.occlusionTexture.strength;
			uniformBufferRaytrace.materialUniformBuffer.emissiveFactor = currentMatrial.emissiveFactor;
			uniformBufferRaytrace.materialUniformBuffer.alphaMode = currentMatrial.alphaMode;
			uniformBufferRaytrace.materialUniformBuffer.alphaCutoff = currentMatrial.alphaCutoff;
			uniformBufferRaytrace.materialUniformBuffer.doubleSided = currentMatrial.doubleSided;

			uniformBufferRaytrace.baseColorTexture = currentMatrial.pbrMetallicRoughness.baseColorTexture.index;
			uniformBufferRaytrace.metallicRoughnessTexture = currentMatrial.pbrMetallicRoughness.metallicRoughnessTexture.index;
			uniformBufferRaytrace.normalTexture = currentMatrial.normalTexture.index;
			uniformBufferRaytrace.occlusionTexture = currentMatrial.occlusionTexture.index;
			uniformBufferRaytrace.emissiveTexture = currentMatrial.emissiveTexture.index;

			materialBuffers.push_back(uniformBufferRaytrace);
		}

		storageBufferResourceCreateInfo = {};
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(RaytraceMaterialUniformBuffer) * glTF.materials.size();
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		storageBufferResourceCreateInfo.data = materialBuffers.data();

		if (!VulkanResource::createStorageBufferResource(physicalDevice, device, queue, commandPool, worldResource->materialStorageBufferResource, storageBufferResourceCreateInfo))
		{
			return false;
		}

		//
		// Gather texture resources.
		//

		std::vector<VkDescriptorImageInfo> descriptorImageInfoTextures;
		for (const Texture& currentTexture : glTF.textures)
		{
			TextureDataResource* currentTextureResource = resourceManager.getTextureResource((uint64_t)&currentTexture);

			VkDescriptorImageInfo descriptorImageInfo = {};

			descriptorImageInfo.sampler = currentTextureResource->textureResource.samplerResource.sampler;
			descriptorImageInfo.imageView = currentTextureResource->textureResource.imageViewResource.imageView;
			descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			descriptorImageInfoTextures.push_back(descriptorImageInfo);
		}

		//
		// Create required descriptor sets
		//

		uint32_t binding = 0;

		const uint32_t accelerationBinding = binding++;
		const uint32_t outputBinding = binding++;

		const uint32_t diffuseBinding = binding++;
		const uint32_t specularBinding = binding++;
		const uint32_t lutBinding = binding++;

		const uint32_t texturesBinding = binding++;
		const uint32_t materialStorageBufferResourcesBinding = binding++;
		const uint32_t primitivesBinding = binding++;

		const uint32_t indicesBinding = binding++;
		const uint32_t positionBinding = binding++;
		const uint32_t normalBinding = binding++;
		const uint32_t tangentBinding = binding++;
		const uint32_t texCoord0Binding = binding++;

		std::vector<VkDescriptorSetLayoutBinding> raytraceDescriptorSetLayoutBindings = {};

		VkDescriptorSetLayoutBinding raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = accelerationBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = outputBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = diffuseBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = specularBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = lutBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		if (descriptorImageInfoTextures.size() > 0)
		{
			raytraceDescriptorSetLayoutBinding = {};
			raytraceDescriptorSetLayoutBinding.binding         = texturesBinding;
			raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			raytraceDescriptorSetLayoutBinding.descriptorCount = static_cast<uint32_t>(descriptorImageInfoTextures.size());
			raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);
		}

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = materialStorageBufferResourcesBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = primitivesBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = indicesBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoIndices.size());
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = positionBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoPosition.size());
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		if (descriptorBufferInfoNormal.size() > 0)
		{
			raytraceDescriptorSetLayoutBinding = {};
			raytraceDescriptorSetLayoutBinding.binding         = normalBinding;
			raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			raytraceDescriptorSetLayoutBinding.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoNormal.size());
			raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);
		}

		if (descriptorBufferInfoTangent.size() > 0)
		{
			raytraceDescriptorSetLayoutBinding = {};
			raytraceDescriptorSetLayoutBinding.binding         = tangentBinding;
			raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			raytraceDescriptorSetLayoutBinding.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoTangent.size());
			raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);
		}

		if (descriptorBufferInfoTexCoord0.size() > 0)
		{
			raytraceDescriptorSetLayoutBinding = {};
			raytraceDescriptorSetLayoutBinding.binding         = texCoord0Binding;
			raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			raytraceDescriptorSetLayoutBinding.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoTexCoord0.size());
			raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);
		}

		VkDescriptorSetLayoutCreateInfo raytraceDescriptorSetLayoutCreateInfo = {};
		raytraceDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		raytraceDescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(raytraceDescriptorSetLayoutBindings.size());
		raytraceDescriptorSetLayoutCreateInfo.pBindings = raytraceDescriptorSetLayoutBindings.data();

		VkResult result = vkCreateDescriptorSetLayout(device, &raytraceDescriptorSetLayoutCreateInfo, nullptr, &worldResource->raytraceDescriptorSetLayout);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		std::vector<VkDescriptorPoolSize> raytraceDescriptorPoolSizes;

		VkDescriptorPoolSize raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		if (descriptorImageInfoTextures.size() > 0)
		{
			raytraceDescriptorPoolSize = {};
			raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			raytraceDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(descriptorImageInfoTextures.size());
			raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);
		}

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoIndices.size());
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoPosition.size());
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		if (descriptorBufferInfoNormal.size() > 0)
		{
			raytraceDescriptorPoolSize = {};
			raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			raytraceDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoNormal.size());
			raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);
		}

		if (descriptorBufferInfoTangent.size() > 0)
		{
			raytraceDescriptorPoolSize = {};
			raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			raytraceDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoTangent.size());
			raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);
		}

		if (descriptorBufferInfoTexCoord0.size() > 0)
		{
			raytraceDescriptorPoolSize = {};
			raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			raytraceDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoTexCoord0.size());
			raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);
		}

		VkDescriptorPoolCreateInfo raytraceDescriptorPoolCreateInfo = {};
		raytraceDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		raytraceDescriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(raytraceDescriptorPoolSizes.size());
		raytraceDescriptorPoolCreateInfo.pPoolSizes = raytraceDescriptorPoolSizes.data();
		raytraceDescriptorPoolCreateInfo.maxSets = 1;

		result = vkCreateDescriptorPool(device, &raytraceDescriptorPoolCreateInfo, nullptr, &worldResource->raytraceDescriptorPool);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		VkDescriptorSetAllocateInfo raytraceDescriptorSetAllocateInfo = {};
		raytraceDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		raytraceDescriptorSetAllocateInfo.descriptorPool = worldResource->raytraceDescriptorPool;
		raytraceDescriptorSetAllocateInfo.descriptorSetCount = 1;
		raytraceDescriptorSetAllocateInfo.pSetLayouts = &worldResource->raytraceDescriptorSetLayout;

		result = vkAllocateDescriptorSets(device, &raytraceDescriptorSetAllocateInfo, &worldResource->raytraceDescriptorSet);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		//
		//

		VkWriteDescriptorSetAccelerationStructureKHR writeDescriptorSetAccelerationStructure = {};
		writeDescriptorSetAccelerationStructure.sType                      = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
		writeDescriptorSetAccelerationStructure.accelerationStructureCount = 1;
		writeDescriptorSetAccelerationStructure.pAccelerationStructures    = &worldResource->topLevelResource.levelResource.accelerationStructureResource.accelerationStructure;

		VkDescriptorImageInfo descriptorImageInfo = {};
		descriptorImageInfo.sampler = VK_NULL_HANDLE;
		descriptorImageInfo.imageView = imageView;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		VkDescriptorBufferInfo descriptorBufferInfo = {};
		descriptorBufferInfo.buffer = worldResource->materialStorageBufferResource.bufferResource.buffer;
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = sizeof(RaytraceMaterialUniformBuffer) * glTF.materials.size();

		VkDescriptorBufferInfo descriptorBufferInfo2 = {};
		descriptorBufferInfo2.buffer = worldResource->instanceResourcesStorageBufferResource.bufferResource.buffer;
		descriptorBufferInfo2.offset = 0;
		descriptorBufferInfo2.range = sizeof(RaytracePrimitiveUniformBuffer) * worldResource->instanceResources.size();

		LightResource* lightResource = resourceManager.getLightResource(worldResource->lightHandle);

		VkDescriptorImageInfo descriptorImageInfoDiffuse = {};
		descriptorImageInfoDiffuse.sampler = lightResource->diffuse.samplerResource.sampler;
		descriptorImageInfoDiffuse.imageView = lightResource->diffuse.imageViewResource.imageView;
		descriptorImageInfoDiffuse.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkDescriptorImageInfo descriptorImageInfoSpecular = {};
		descriptorImageInfoSpecular.sampler = lightResource->specular.samplerResource.sampler;
		descriptorImageInfoSpecular.imageView = lightResource->specular.imageViewResource.imageView;
		descriptorImageInfoSpecular.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkDescriptorImageInfo descriptorImageInfoLut = {};
		descriptorImageInfoLut.sampler = lightResource->lut.samplerResource.sampler;
		descriptorImageInfoLut.imageView = lightResource->lut.imageViewResource.imageView;
		descriptorImageInfoLut.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		std::vector<VkWriteDescriptorSet> writeDescriptorSets;

		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet          = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding      = accelerationBinding;
		writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pNext           = &writeDescriptorSetAccelerationStructure;	// Chained
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = outputBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pImageInfo = &descriptorImageInfo;
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = diffuseBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pImageInfo = &descriptorImageInfoDiffuse;
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = specularBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pImageInfo = &descriptorImageInfoSpecular;
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = lutBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pImageInfo = &descriptorImageInfoLut;
		writeDescriptorSets.push_back(writeDescriptorSet);

		if (descriptorImageInfoTextures.size() > 0)
		{
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
			writeDescriptorSet.dstBinding = texturesBinding;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptorImageInfoTextures.size());
			writeDescriptorSet.pImageInfo = descriptorImageInfoTextures.data();
			writeDescriptorSets.push_back(writeDescriptorSet);
		}

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = materialStorageBufferResourcesBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = primitivesBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pBufferInfo = &descriptorBufferInfo2;
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = indicesBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoIndices.size());
		writeDescriptorSet.pBufferInfo = descriptorBufferInfoIndices.data();
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = positionBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoPosition.size());
		writeDescriptorSet.pBufferInfo = descriptorBufferInfoPosition.data();
		writeDescriptorSets.push_back(writeDescriptorSet);

		if (descriptorBufferInfoNormal.size() > 0)
		{
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
			writeDescriptorSet.dstBinding = normalBinding;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoNormal.size());
			writeDescriptorSet.pBufferInfo = descriptorBufferInfoNormal.data();
			writeDescriptorSets.push_back(writeDescriptorSet);
		}

		if (descriptorBufferInfoTangent.size() > 0)
		{
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
			writeDescriptorSet.dstBinding = tangentBinding;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoTangent.size());
			writeDescriptorSet.pBufferInfo = descriptorBufferInfoTangent.data();
			writeDescriptorSets.push_back(writeDescriptorSet);
		}

		if (descriptorBufferInfoTexCoord0.size() > 0)
		{
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
			writeDescriptorSet.dstBinding = texCoord0Binding;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoTexCoord0.size());
			writeDescriptorSet.pBufferInfo = descriptorBufferInfoTexCoord0.data();
			writeDescriptorSets.push_back(writeDescriptorSet);
		}

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

		//
		// Creating ray tracing raytracePipeline.
		//

		VkPushConstantRange pushConstantRange = {};
	    pushConstantRange.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	    pushConstantRange.offset = 0;
	    pushConstantRange.size = sizeof(RaytraceUniformPushConstant);

		VkPipelineLayoutCreateInfo raytracePipelineLayoutCreateInfo = {};
		raytracePipelineLayoutCreateInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		raytracePipelineLayoutCreateInfo.setLayoutCount = 1;
		raytracePipelineLayoutCreateInfo.pSetLayouts    = &worldResource->raytraceDescriptorSetLayout;
		raytracePipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		raytracePipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

		result = vkCreatePipelineLayout(device, &raytracePipelineLayoutCreateInfo, nullptr, &worldResource->raytracePipelineLayout);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		//
		// Build the required raytracing shaders.
		//

		std::map<std::string, std::string> macros;

		macros["ACCELERATION_BINDING"] = std::to_string(accelerationBinding);
		macros["OUTPUT_BINDING"] = std::to_string(outputBinding);

		macros["DIFFUSE_BINDING"] = std::to_string(diffuseBinding);
		macros["SPECULAR_BINDING"] = std::to_string(specularBinding);
		macros["LUT_BINDING"] = std::to_string(lutBinding);

		macros["TEXTURES_BINDING"] = std::to_string(texturesBinding);
		macros["MATERIALS_BINDING"] = std::to_string(materialStorageBufferResourcesBinding);
		macros["PRIMITIVES_BINDING"] = std::to_string(primitivesBinding);

		macros["INDICES_BINDING"] = std::to_string(indicesBinding);
		macros["POSITION_BINDING"] = std::to_string(positionBinding);
		macros["NORMAL_BINDING"] = std::to_string(normalBinding);
		macros["TANGENT_BINDING"] = std::to_string(tangentBinding);
		macros["TEXCOORD0_BINDING"] = std::to_string(texCoord0Binding);

		if (descriptorImageInfoTextures.size() > 0)
		{
			macros["HAS_TEXTURES"] = "";
		}

		if (descriptorBufferInfoNormal.size() > 0)
		{
			macros["NORMAL_VEC3"] = "";
		}
		if (descriptorBufferInfoTangent.size() > 0)
		{
			macros["TANGENT_VEC4"] = "";
		}
		if (descriptorBufferInfoTexCoord0.size() > 0)
		{
			macros["TEXCOORD0_VEC2"] = "";
		}

		std::string rayGenShaderSource = "";
		if (!FileIO::open(rayGenShaderSource, "../Resources/shaders/gltf.rgen"))
		{
			return false;
		}

		std::vector<uint32_t> rayGenShaderCode;
		if (!Compiler::buildShader(rayGenShaderCode, rayGenShaderSource, macros, shaderc_raygen_shader))
		{
			return false;
		}

		if (!VulkanResource::createShaderModule(worldResource->rayGenShaderModule, device, rayGenShaderCode))
		{
			return false;
		}

		//

		std::string missShaderSource = "";
		if (!FileIO::open(missShaderSource, "../Resources/shaders/gltf.rmiss"))
		{
			return false;
		}

		std::vector<uint32_t> missShaderCode;
		if (!Compiler::buildShader(missShaderCode, missShaderSource, macros, shaderc_miss_shader))
		{
			return false;
		}

		if (!VulkanResource::createShaderModule(worldResource->missShaderModule, device, missShaderCode))
		{
			return false;
		}

		//

		std::string closestHitShaderSource = "";
		if (!FileIO::open(closestHitShaderSource, "../Resources/shaders/gltf.rchit"))
		{
			return false;
		}

		std::vector<uint32_t> closestHitShaderCode;
		if (!Compiler::buildShader(closestHitShaderCode, closestHitShaderSource, macros, shaderc_closesthit_shader))
		{
			return false;
		}

		if (!VulkanResource::createShaderModule(worldResource->closestHitShaderModule, device, closestHitShaderCode))
		{
			return false;
		}

		//
		//

		std::vector<VkPipelineShaderStageCreateInfo> raytracePipelineShaderStageCreateInfos;
		raytracePipelineShaderStageCreateInfos.resize(3);

		raytracePipelineShaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		raytracePipelineShaderStageCreateInfos[0].stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		raytracePipelineShaderStageCreateInfos[0].module = worldResource->rayGenShaderModule;
		raytracePipelineShaderStageCreateInfos[0].pName = "main";

		raytracePipelineShaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		raytracePipelineShaderStageCreateInfos[1].stage = VK_SHADER_STAGE_MISS_BIT_KHR;
		raytracePipelineShaderStageCreateInfos[1].module = worldResource->missShaderModule;
		raytracePipelineShaderStageCreateInfos[1].pName = "main";

		raytracePipelineShaderStageCreateInfos[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		raytracePipelineShaderStageCreateInfos[2].stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		raytracePipelineShaderStageCreateInfos[2].module = worldResource->closestHitShaderModule;
		raytracePipelineShaderStageCreateInfos[2].pName = "main";

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
		rayTracingPipelineCreateInfo.stageCount        = static_cast<uint32_t>(raytracePipelineShaderStageCreateInfos.size());
		rayTracingPipelineCreateInfo.pStages           = raytracePipelineShaderStageCreateInfos.data();
		rayTracingPipelineCreateInfo.groupCount        = static_cast<uint32_t>(rayTracingShaderGroupCreateInfos.size());
		rayTracingPipelineCreateInfo.pGroups           = rayTracingShaderGroupCreateInfos.data();
		rayTracingPipelineCreateInfo.maxRecursionDepth = 2;
		rayTracingPipelineCreateInfo.layout            = worldResource->raytracePipelineLayout;
		rayTracingPipelineCreateInfo.libraries.sType   = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR;

		result = vkCreateRayTracingPipelinesKHR(device, VK_NULL_HANDLE, 1, &rayTracingPipelineCreateInfo, nullptr, &worldResource->raytracePipeline);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		//
		// Build up shader binding table.
		//

		VkPhysicalDeviceProperties2 physicalDeviceProperties2 = {};
		physicalDeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		VkPhysicalDeviceRayTracingPropertiesKHR physicalDeviceRayTracingProperties = {};
		physicalDeviceRayTracingProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_KHR;

		physicalDeviceProperties2.pNext = &physicalDeviceRayTracingProperties;
		vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);

		bufferResourceCreateInfo = {};
		bufferResourceCreateInfo.size = physicalDeviceRayTracingProperties.shaderGroupHandleSize * rayTracingShaderGroupCreateInfos.size();
		bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR;
		bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		if (!VulkanResource::createBufferResource(physicalDevice, device, worldResource->shaderBindingBufferResource, bufferResourceCreateInfo))
		{
			return false;
		}
		worldResource->size = physicalDeviceRayTracingProperties.shaderGroupHandleSize;

		std::vector<uint8_t> rayTracingShaderGroupHandles(bufferResourceCreateInfo.size);

		result = vkGetRayTracingShaderGroupHandlesKHR(device, worldResource->raytracePipeline, 0, static_cast<uint32_t>(rayTracingShaderGroupCreateInfos.size()), rayTracingShaderGroupHandles.size(), rayTracingShaderGroupHandles.data());
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		if (!VulkanResource::copyHostToDevice(device, worldResource->shaderBindingBufferResource, rayTracingShaderGroupHandles.data(), rayTracingShaderGroupHandles.size()))
		{
			return false;
		}
	}

	return true;
}

void AllocationManager::terminate(VkDevice device)
{
	resourceManager.terminate(device);
}
