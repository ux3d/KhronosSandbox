#include "Application.h"

// Private

struct VertexData {
	glm::vec4 position;
	glm::vec2 texCoord;
};

struct UniformData {
	int32_t tonemap;
	int32_t transferFunction;
	int32_t imageSrgbNonLinear;
	int32_t debug;
	float minLuminance;
	float maxLuminance;
};

bool Application::applicationInit()
{
	TextureResourceCreateInfo textureResourceCreateInfo = {};

	switch (testImage)
	{
		case 0:
			if (!ImageDataIO::open(textureResourceCreateInfo.imageDataResources, "../Resources/images/field.hdr"))
			{
				return false;
			}
			imageSrgbNonLinear = false;		// HDR is linear and we assume it is in the Linear SRGB space, which has the same gamut as Rec.709
		break;
		case 1:
			if (!ImageDataIO::open(textureResourceCreateInfo.imageDataResources, "../Resources/images/desert.jpg"))
			{
				return false;
			}
			imageSrgbNonLinear = true;			// JPG in general is Non-linear SRGB and we are uploading it 1:1 with no specific format conversion
		break;
		default:
			return false;
	}

	//

	if (surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
	{
		if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
		{
			transferFunction = 1;	// Format: B8G8R8A8_UNORM and ColorSpace: SRGB_NONLINEAR => sRGB transfer function
		}
		else if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB)
		{
			transferFunction = 0;	// Format: B8G8R8A8_SRG and ColorSpace: SRGB_NONLINEAR => No transfer function
		}
		else if (surfaceFormat.format == VK_FORMAT_A2B10G10R10_UNORM_PACK32)
		{
			transferFunction = 1;	// Format: A2B10G10R10_UNORM_PACK32 and ColorSpace: SRGB_NONLINEAR => sRGB transfer function
		}
		else
		{
			// Unknown combination

			return false;
		}
	}
	else if (surfaceFormat.colorSpace == VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT)
	{
		if (surfaceFormat.format == VK_FORMAT_R16G16B16A16_SFLOAT)
		{
			transferFunction = 0;	// Format: R16G16B16A16_SFLOAT and ColorSpace: EXTENDED_SRGB_LINEAR => No transfer function
		}
		else
		{
			// Unknown combination

			return false;
		}
	}
	else if (surfaceFormat.colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT)
	{
		if (surfaceFormat.format == VK_FORMAT_A2B10G10R10_UNORM_PACK32)
		{
			transferFunction = 2;	// Format: FORMAT_A2B10G10R10_UNORM_PACK32 and ColorSpace: COLOR_SPACE_HDR10_ST2084 => PQ transfer function
		}
		else
		{
			// Unknown combination

			return false;
		}
	}
	else
	{
		// Unknown combination

		return false;
	}

	textureResourceCreateInfo.samplerResourceCreateInfo.magFilter = VK_FILTER_LINEAR;
	textureResourceCreateInfo.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, textureResource, textureResourceCreateInfo))
	{
		return false;
	}

	//

	std::map<std::string, std::string> macros;

	//

	std::string vertexShaderSource = "";
	if (!FileIO::open(vertexShaderSource, "../Resources/shaders/tm_tf.vert"))
	{
		return false;
	}

	std::vector<uint32_t> vertexShaderCode;
	if (!Compiler::buildShader(vertexShaderCode, vertexShaderSource, macros, shaderc_vertex_shader))
	{
		return false;
	}

	//

	std::string fragmentShaderSource = "";
	if (!FileIO::open(fragmentShaderSource, "../Resources/shaders/tm_tf.frag"))
	{
		return false;
	}

	std::vector<uint32_t> fragmentShaderCode;
	if (!Compiler::buildShader(fragmentShaderCode, fragmentShaderSource, macros, shaderc_fragment_shader))
	{
		return false;
	}

	//

	if (!VulkanResource::createShaderModule(vertexShaderModule, device, vertexShaderCode))
	{
		return false;
	}

	if (!VulkanResource::createShaderModule(fragmentShaderModule, device, fragmentShaderCode))
	{
		return false;
	}


	//

	UniformBufferResourceCreateInfo uniformBufferResourceCreateInfo = {};

	uniformBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(UniformData);
	uniformBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	uniformBufferResources.resize(commandBuffers.size());
	for (auto& currentUniformBufferResource : uniformBufferResources)
	{
		if (!VulkanResource::createUniformBufferResource(physicalDevice, device, currentUniformBufferResource, uniformBufferResourceCreateInfo))
		{
			return false;
		}
	}

	//

	VkResult result = VK_SUCCESS;

	VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[2] = {};
	descriptorSetLayoutBindings[0].binding = 0;
	descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorSetLayoutBindings[0].descriptorCount = 1;
	descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	descriptorSetLayoutBindings[1].binding = 1;
	descriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorSetLayoutBindings[1].descriptorCount = 1;
	descriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = 2;
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings;

	result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	VkDescriptorPoolSize descriptorPoolSizes[2] = {};
	descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSizes[0].descriptorCount = 1;

	descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSizes[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.poolSizeCount = 2;
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;
	descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(commandBuffers.size());

	result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

	descriptorSets.resize(commandBuffers.size());
	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSets[i]);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		VkDescriptorBufferInfo descriptorBufferInfo = {};
		descriptorBufferInfo.buffer = uniformBufferResources[i].bufferResource.buffer;
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = sizeof(UniformData);

		VkDescriptorImageInfo descriptorImageInfo = {};
		descriptorImageInfo.sampler = textureResource.samplerResource.sampler;
		descriptorImageInfo.imageView = textureResource.imageViewResource.imageView;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet writeDescriptorSets[2] = {};
		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet = descriptorSets[i];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfo;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet = descriptorSets[i];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].pImageInfo = &descriptorImageInfo;

		vkUpdateDescriptorSets(device, 2, writeDescriptorSets, 0, nullptr);
	}

	//

	VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[2] = {};

	pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	pipelineShaderStageCreateInfo[0].module = vertexShaderModule;
	pipelineShaderStageCreateInfo[0].pName = "main";

	pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	pipelineShaderStageCreateInfo[1].module = fragmentShaderModule;
	pipelineShaderStageCreateInfo[1].pName = "main";

	//

	std::vector<uint16_t> vertexIndices = {
	    0, 1, 2, 2, 3, 0
	};

	VertexBufferResourceCreateInfo vertexIndexBufferResourceCreateInfo = {};

	vertexIndexBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(vertexIndices[0]) * 6;
	vertexIndexBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	vertexIndexBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	vertexIndexBufferResourceCreateInfo.data = vertexIndices.data();

	if (!VulkanResource::createVertexBufferResource(physicalDevice, device, queue, commandPool, vertexIndexBufferResource, vertexIndexBufferResourceCreateInfo))
	{
		return false;
	}

	//

	VertexData vertexData[4] = {
		{glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
		{glm::vec4( 1.0f, -1.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
		{glm::vec4( 1.0f,  1.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
		{glm::vec4( -1.0f,  1.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)}
	};

	VertexBufferResourceCreateInfo vertexBufferResourceCreateInfo = {};

	vertexBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(vertexData[0]) * 4;
	vertexBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	vertexBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	vertexBufferResourceCreateInfo.data = vertexData;

	if (!VulkanResource::createVertexBufferResource(physicalDevice, device, queue, commandPool, vertexBufferResource, vertexBufferResourceCreateInfo))
	{
		return false;
	}

	VkVertexInputBindingDescription vertexInputBindingDescription = {};
	vertexInputBindingDescription.binding = 0;
	vertexInputBindingDescription.stride = sizeof(VertexData);
	vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription vertexInputAttributeDescription[2] = {};
	vertexInputAttributeDescription[0].binding = 0;
	vertexInputAttributeDescription[0].location = 0;
	vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	vertexInputAttributeDescription[0].offset = offsetof(VertexData, position);
	vertexInputAttributeDescription[1].binding = 0;
	vertexInputAttributeDescription[1].location = 1;
	vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32_SFLOAT;
	vertexInputAttributeDescription[1].offset = offsetof(VertexData, texCoord);

	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2;
	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescription;

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
	pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

	//

	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
	pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineMultisampleStateCreateInfo.rasterizationSamples = samples;
	pipelineMultisampleStateCreateInfo.minSampleShading = 1.0f;

	//

	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {};
	pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
	pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipelineColorBlendStateCreateInfo.attachmentCount = 1;
	pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;

	//

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

	result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
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
	graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
	graphicsPipelineCreateInfo.layout = pipelineLayout;
	graphicsPipelineCreateInfo.renderPass = renderPass;

	result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	return true;
}

bool Application::applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime)
{
	VkClearColorValue clearColorValue = {};
	clearColorValue.float32[0] = 0.0f;
	clearColorValue.float32[1] = 0.0f;
	clearColorValue.float32[2] = 0.0f;
	clearColorValue.float32[3] = 1.0f;

	VkClearValue clearValues[1] = {};
	clearValues[0].color = clearColorValue;

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = framebuffers[frameIndex];
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent = {width, height};
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = clearValues;

	vkCmdBeginRenderPass(commandBuffers[frameIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	UniformData uniformData = {};
	uniformData.tonemap = tonemap;
	uniformData.transferFunction = transferFunction;
	uniformData.imageSrgbNonLinear = (int32_t)imageSrgbNonLinear;
	uniformData.debug = (int32_t)debug;
	uniformData.minLuminance = hdrMetadata.minLuminance;
	// Use half the range.
	uniformData.maxLuminance = hdrMetadata.maxLuminance * 0.5f;

	if (!VulkanResource::copyHostToDevice(device, uniformBufferResources[frameIndex].bufferResource, &uniformData, sizeof(uniformData)))
	{
		return false;
	}

	vkCmdBindDescriptorSets(commandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[frameIndex], 0, nullptr);

	vkCmdBindIndexBuffer(commandBuffers[frameIndex], vertexIndexBufferResource.bufferResource.buffer, 0, VK_INDEX_TYPE_UINT16);

	VkBuffer vertexBuffers[] = {vertexBufferResource.bufferResource.buffer};
	VkDeviceSize vertexBuffersOffsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffers[frameIndex], 0, 1, vertexBuffers, vertexBuffersOffsets);

	vkCmdDrawIndexed(commandBuffers[frameIndex], 6, 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffers[frameIndex]);

	return true;
}

void Application::applicationTerminate()
{
	for (auto& currentUniformBufferResource : uniformBufferResources)
	{
		VulkanResource::destroyUniformBufferResource(device, currentUniformBufferResource);
	}

	VulkanResource::destroyVertexBufferResource(device, vertexBufferResource);
	VulkanResource::destroyVertexBufferResource(device, vertexIndexBufferResource);

	VulkanResource::destroyTextureResource(device, textureResource);

	if (graphicsPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		graphicsPipeline = VK_NULL_HANDLE;
	}

	if (pipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		pipelineLayout = VK_NULL_HANDLE;
	}

	// Descriptor sets do not have to be freed, as managed by pool.
	descriptorSets.clear();

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

	if (vertexShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, vertexShaderModule, nullptr);
		vertexShaderModule = VK_NULL_HANDLE;
	}

	if (fragmentShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
		fragmentShaderModule = VK_NULL_HANDLE;
	}
}

// Public

Application::Application(int32_t tonemap, int32_t testImage, const VkHdrMetadataEXT& hdrMetadata, bool debug) :
	tonemap(tonemap), testImage(testImage), hdrMetadata(hdrMetadata), debug(debug)
{
}

Application::~Application()
{
}
