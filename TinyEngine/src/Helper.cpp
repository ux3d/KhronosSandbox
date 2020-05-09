#include "Helper.h"

#include "generic/Logger.h"

#include <cmath>
#include <cstring>

#include <glm/glm.hpp>

bool Helper::isDepthStencilFormat(VkFormat format)
{
	switch (format)
	{
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;
		default:
			break;
	}

	return false;
}

bool Helper::getFormat(VkFormat& format, uint32_t componentTypeSize, bool componentTypeSigned, bool componentTypeInteger, uint32_t typeCount)
{
	bool result = false;

	if (componentTypeInteger)
	{
		if (!componentTypeSigned)
		{
			if (componentTypeSize == 1)
			{
				switch (typeCount)
				{
					case 1:
						// TODO
						break;
					case 2:
						// TODO
						break;
					case 3:
						// TODO
						break;
					case 4:
						// TODO
						break;
					default:
						break;
				}
			}
			else if (componentTypeSize == 2)
			{
				switch (typeCount)
				{
					case 1:
						// TODO
						break;
					case 2:
						// TODO
						break;
					case 3:
						// TODO
						break;
					case 4:
						// TODO
						break;
					default:
						break;
				}
			}
		}
	}
	else
	{
		if (componentTypeSigned)
		{
			if (componentTypeSize == 2)
			{
				switch (typeCount)
				{
					case 1:
						format = VK_FORMAT_R16_SFLOAT;
						result = true;
						break;
					case 2:
						format = VK_FORMAT_R16G16_SFLOAT;
						result = true;
						break;
					case 3:
						format = VK_FORMAT_R16G16B16_SFLOAT;
						result = true;
						break;
					case 4:
						format = VK_FORMAT_R16G16B16A16_SFLOAT;
						result = true;
						break;
					default:
						break;
				}
			}
			else if (componentTypeSize == 4)
			{
				switch (typeCount)
				{
					case 1:
						format = VK_FORMAT_R32_SFLOAT;
						result = true;
						break;
					case 2:
						format = VK_FORMAT_R32G32_SFLOAT;
						result = true;
						break;
					case 3:
						format = VK_FORMAT_R32G32B32_SFLOAT;
						result = true;
						break;
					case 4:
						format = VK_FORMAT_R32G32B32A32_SFLOAT;
						result = true;
						break;
					default:
						break;
				}
			}
		}
	}

	return result;
}

bool Helper::findMemoryTypeIndex(uint32_t& memoryTypeIndex, VkPhysicalDevice physicalDevice, uint32_t memoryType, VkMemoryPropertyFlags memoryProperty)
{
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = {};
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

	for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
	    if ((memoryType & (1 << i)) && (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperty) == memoryProperty)
	    {
	    	memoryTypeIndex = i;

	        return true;
	    }
	}

	return false;
}

bool Helper::beginOneTimeSubmitCommand(VkDevice device, VkCommandPool commandPool, VkCommandBuffer& commandBuffer)
{
	VkResult result = VK_SUCCESS;

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.commandBufferCount = 1;

	result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

		return false;
	}

	return true;
}

bool Helper::endOneTimeSubmitCommand(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkCommandBuffer commandBuffer)
{
	VkResult result = VK_SUCCESS;

	result = vkEndCommandBuffer(commandBuffer);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

		return false;
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

		return false;
	}

	result = vkQueueWaitIdle(queue);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

		return false;
	}

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

	return true;
}

bool Helper::transitionImageLayout(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t baseMipLevel, uint32_t levelCount, uint32_t layerCount)
{
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	if (!beginOneTimeSubmitCommand(device, commandPool, commandBuffer))
	{
		return false;
	}

	//

	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.oldLayout = oldLayout;
	imageMemoryBarrier.newLayout = newLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.baseMipLevel = baseMipLevel;
	imageMemoryBarrier.subresourceRange.levelCount = levelCount;
	imageMemoryBarrier.subresourceRange.layerCount = layerCount;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

	    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	    sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

	    sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
	    return false;
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	//

	if (!endOneTimeSubmitCommand(device, queue, commandPool, commandBuffer))
	{
		return false;
	}

	return true;
}

bool Helper::copyBuffer(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	if (!beginOneTimeSubmitCommand(device, commandPool, commandBuffer))
	{
		return false;
	}

	//

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	//

	if (!endOneTimeSubmitCommand(device, queue, commandPool, commandBuffer))
	{
		return false;
	}

	return true;
}

bool Helper::copyBufferToImage(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkBuffer srcBuffer, VkImage dstImage, uint32_t width, uint32_t height, uint32_t mipLevel, uint32_t baseArrayLayer)
{
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	if (!beginOneTimeSubmitCommand(device, commandPool, commandBuffer))
	{
		return false;
	}

	//

	VkBufferImageCopy bufferImageCopy = {};
	bufferImageCopy.bufferRowLength = width;
	bufferImageCopy.bufferImageHeight = height;
	bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	bufferImageCopy.imageSubresource.mipLevel = mipLevel;
	bufferImageCopy.imageSubresource.baseArrayLayer = baseArrayLayer;
	bufferImageCopy.imageSubresource.layerCount = 1;
	bufferImageCopy.imageExtent = {width, height, 1};

	vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);

	//

	if (!endOneTimeSubmitCommand(device, queue, commandPool, commandBuffer))
	{
		return false;
	}

	return true;
}

bool Helper::generateMipMap(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImage image, uint32_t width, uint32_t height, uint32_t levelCount, uint32_t layerCount)
{
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	//

	if (!transitionImageLayout(device, queue, commandPool, image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 0, 1, layerCount))
	{
		return false;
	}

	for (uint32_t mipLevel = 1; mipLevel < levelCount; mipLevel++)
	{
		for (uint32_t baseArrayLayer = 0; baseArrayLayer < layerCount; baseArrayLayer++)
		{
			if (!transitionImageLayout(device, queue, commandPool, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevel, 1, layerCount))
			{
				return false;
			}

			//

			if (!beginOneTimeSubmitCommand(device, commandPool, commandBuffer))
			{
				return false;
			}

			//

			VkImageBlit blit = {};
			blit.srcOffsets[1] = { (int32_t)width, (int32_t)height, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = 0;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;

			blit.dstOffsets[1] = { glm::max((int32_t)width >> mipLevel, 1), glm::max((int32_t)height >> mipLevel, 1), 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = mipLevel;
			blit.dstSubresource.baseArrayLayer = baseArrayLayer;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

			//

			if (!endOneTimeSubmitCommand(device, queue, commandPool, commandBuffer))
			{
				return false;
			}

			//

			if (!transitionImageLayout(device, queue, commandPool, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevel, 1, layerCount))
			{
				return false;
			}
		}
	}

	if (!transitionImageLayout(device, queue, commandPool, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, layerCount))
	{
		return false;
	}

	return true;
}

bool Helper::copyHostToDevice(VkDevice device, BufferResource& bufferResource, const void* data, size_t size)
{
	if (data == nullptr || size == 0)
	{
		return false;
	}

	VkResult result = VK_SUCCESS;

	void* mappedData = nullptr;

	result = vkMapMemory(device, bufferResource.deviceMemory, 0, size, 0, (void**)&mappedData);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	memcpy(mappedData, data, size);

	vkUnmapMemory(device, bufferResource.deviceMemory);

	return true;
}

bool Helper::createBufferResource(VkPhysicalDevice physicalDevice, VkDevice device, BufferResource& bufferResource, const BufferResourceCreateInfo& bufferResourceCreateInfo)
{
	VkResult result = VK_SUCCESS;

	bool needStaging = false;
	if ((bufferResourceCreateInfo.memoryProperty & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	{
		needStaging = true;
	}

	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = bufferResourceCreateInfo.size;
	bufferCreateInfo.usage = bufferResourceCreateInfo.usage;
	if (needStaging)
	{
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}

	result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &bufferResource.buffer);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	//

	VkMemoryRequirements memoryRequirements = {};
	vkGetBufferMemoryRequirements(device, bufferResource.buffer, &memoryRequirements);

	uint32_t memoryTypeIndex = 0;
	if (!Helper::findMemoryTypeIndex(memoryTypeIndex, physicalDevice, memoryRequirements.memoryTypeBits, bufferResourceCreateInfo.memoryProperty))
	{
		return false;
	}

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = bufferResourceCreateInfo.pNext;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

	VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo = {};
	memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	if ((bufferResourceCreateInfo.usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) == VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
	{
		memoryAllocateFlagsInfo.flags |= VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

		memoryAllocateInfo.pNext = &memoryAllocateFlagsInfo;
	}

	result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &bufferResource.deviceMemory);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	result = vkBindBufferMemory(device, bufferResource.buffer, bufferResource.deviceMemory, 0);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	return true;
}

void Helper::destroyBufferResource(VkDevice device, BufferResource& bufferResource)
{
	if (bufferResource.deviceMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(device, bufferResource.deviceMemory, nullptr);
		bufferResource.deviceMemory = VK_NULL_HANDLE;
	}

	if (bufferResource.buffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(device, bufferResource.buffer, nullptr);
		bufferResource.buffer = VK_NULL_HANDLE;
	}
}

bool Helper::createShaderModule(VkShaderModule& shaderModule, VkDevice device, const std::vector<uint32_t>& code)
{
	VkResult result = VK_SUCCESS;

	VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.codeSize = code.size() * 4;
	shaderModuleCreateInfo.pCode = code.data();

	result = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	return true;
}

bool Helper::createDeviceBufferResource(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, DeviceBufferResource& deviceBufferResource, const DeviceBufferResourceCreateInfo& deviceBufferResourceCreateInfo)
{
	if (!createBufferResource(physicalDevice, device, deviceBufferResource.bufferResource, deviceBufferResourceCreateInfo.bufferResourceCreateInfo))
	{
		return false;
	}

	//

	if (deviceBufferResourceCreateInfo.data)
	{
		bool needStaging = false;
		if ((deviceBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
		{
			needStaging = true;
		}

		if (needStaging)
		{
			DeviceBufferResource stageDeviceBufferResource = {};
			DeviceBufferResourceCreateInfo stageDeviceBufferResourceCreateInfo = deviceBufferResourceCreateInfo;
			stageDeviceBufferResourceCreateInfo.bufferResourceCreateInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			stageDeviceBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

			if (!createDeviceBufferResource(physicalDevice, device, queue, commandPool, stageDeviceBufferResource, stageDeviceBufferResourceCreateInfo))
			{
				return false;
			}

			if (!copyBuffer(device, queue, commandPool, stageDeviceBufferResource.bufferResource.buffer, deviceBufferResource.bufferResource.buffer, deviceBufferResourceCreateInfo.bufferResourceCreateInfo.size))
			{
				destroyDeviceBufferResource(device, stageDeviceBufferResource);

				return false;
			}

			destroyDeviceBufferResource(device, stageDeviceBufferResource);
		}
		else
		{
			if ((deviceBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) != (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
			{
				return false;
			}

			if (!copyHostToDevice(device, deviceBufferResource.bufferResource, deviceBufferResourceCreateInfo.data, deviceBufferResourceCreateInfo.bufferResourceCreateInfo.size))
			{
				return false;
			}
		}
	}

	return true;
}

void Helper::destroyDeviceBufferResource(VkDevice device, DeviceBufferResource& deviceBufferResource)
{
	destroyBufferResource(device, deviceBufferResource.bufferResource);
}


bool Helper::createVertexBufferResource(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VertexBufferResource& vertexBufferResource, const VertexBufferResourceCreateInfo& vertexBufferResourceCreateInfo)
{
	return createDeviceBufferResource(physicalDevice, device, queue, commandPool, vertexBufferResource, vertexBufferResourceCreateInfo);
}

void Helper::destroyVertexBufferResource(VkDevice device, VertexBufferResource& vertexBufferResource)
{
	destroyDeviceBufferResource(device, vertexBufferResource);
}

bool Helper::createStorageBufferResource(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, StorageBufferResource& storageBufferResource, const StorageBufferResourceCreateInfo& storageBufferResourceCreateInfo)
{
	return createDeviceBufferResource(physicalDevice, device, queue, commandPool, storageBufferResource, storageBufferResourceCreateInfo);
}

void Helper::destroyStorageBufferResource(VkDevice device, StorageBufferResource& storageBufferResource)
{
	destroyDeviceBufferResource(device, storageBufferResource);
}

bool Helper::createUniformBufferResource(VkPhysicalDevice physicalDevice, VkDevice device, UniformBufferResource& uniformBufferResource, const UniformBufferResourceCreateInfo& uniformBufferResourceCreateInfo)
{
	if ((uniformBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) != (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
	{
		return false;
	}

	if (!createBufferResource(physicalDevice, device, uniformBufferResource.bufferResource, uniformBufferResourceCreateInfo.bufferResourceCreateInfo))
	{
		return false;
	}

	if (uniformBufferResourceCreateInfo.data)
	{
		if (!copyHostToDevice(device, uniformBufferResource.bufferResource, uniformBufferResourceCreateInfo.data, uniformBufferResourceCreateInfo.bufferResourceCreateInfo.size))
		{
			return false;
		}
	}

	return true;
}

void Helper::destroyUniformBufferResource(VkDevice device, UniformBufferResource& uniformBufferResource)
{
	destroyBufferResource(device, uniformBufferResource.bufferResource);
}

bool Helper::createImageViewResource(VkPhysicalDevice physicalDevice, VkDevice device, ImageViewResource& imageViewResource, const ImageViewResourceCreateInfo& imageViewResourceCreateInfo)
{
	VkResult result = VK_SUCCESS;

	uint32_t arrayLayers = 1;
	if ((imageViewResourceCreateInfo.flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) == VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
	{
		arrayLayers = 6;
	}

	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.flags = imageViewResourceCreateInfo.flags;
	imageCreateInfo.imageType = imageViewResourceCreateInfo.imageType;
	imageCreateInfo.format = imageViewResourceCreateInfo.format;
	imageCreateInfo.extent = imageViewResourceCreateInfo.extent;
	imageCreateInfo.mipLevels = imageViewResourceCreateInfo.mipLevels;
	imageCreateInfo.arrayLayers = arrayLayers;
	imageCreateInfo.samples = imageViewResourceCreateInfo.samples;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = imageViewResourceCreateInfo.usage;

	result = vkCreateImage(device, &imageCreateInfo, nullptr, &imageViewResource.image);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	//

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(device, imageViewResource.image, &memoryRequirements);

	uint32_t memoryTypeIndex = 0;
	if (!Helper::findMemoryTypeIndex(memoryTypeIndex, physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
	{
		return false;
	}

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

	result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &imageViewResource.deviceMemory);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	result = vkBindImageMemory(device, imageViewResource.image, imageViewResource.deviceMemory, 0);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	//

	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = imageViewResource.image;
	imageViewCreateInfo.viewType = imageViewResourceCreateInfo.imageViewType;
	imageViewCreateInfo.format = imageViewResourceCreateInfo.format;
	imageViewCreateInfo.subresourceRange.aspectMask = imageViewResourceCreateInfo.aspectMask;
	imageViewCreateInfo.subresourceRange.levelCount = imageViewResourceCreateInfo.mipLevels;
	imageViewCreateInfo.subresourceRange.layerCount = arrayLayers;

	result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViewResource.imageView);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	return true;
}

void Helper::destroyImageViewResource(VkDevice device, ImageViewResource& imageViewResource)
{
	if (imageViewResource.deviceMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(device, imageViewResource.deviceMemory, nullptr);
		imageViewResource.deviceMemory = VK_NULL_HANDLE;
	}

	if (imageViewResource.imageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(device, imageViewResource.imageView, nullptr);
		imageViewResource.imageView = VK_NULL_HANDLE;
	}

	if (imageViewResource.image != VK_NULL_HANDLE)
	{
		vkDestroyImage(device, imageViewResource.image, nullptr);
		imageViewResource.image = VK_NULL_HANDLE;
	}
}

bool Helper::createTextureResource(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, TextureResource& textureResource, const TextureResourceCreateInfo& textureResourceCreateInfo)
{
	if (textureResourceCreateInfo.imageDataResources.images.size() != textureResourceCreateInfo.imageDataResources.mipLevels * textureResourceCreateInfo.imageDataResources.faceCount)
	{
		return false;
	}

	//

	ImageViewResourceCreateInfo imageViewResourceCreateInfo = {};
	imageViewResourceCreateInfo.format = textureResourceCreateInfo.imageDataResources.images[0].format;
	imageViewResourceCreateInfo.extent = {textureResourceCreateInfo.imageDataResources.images[0].width, textureResourceCreateInfo.imageDataResources.images[0].height, 1};
	imageViewResourceCreateInfo.mipLevels = textureResourceCreateInfo.imageDataResources.mipLevels;
	imageViewResourceCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageViewResourceCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageViewResourceCreateInfo.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	if (textureResourceCreateInfo.imageDataResources.faceCount == 6)
	{
		imageViewResourceCreateInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		imageViewResourceCreateInfo.imageViewType = VK_IMAGE_VIEW_TYPE_CUBE;
	}

	bool creatMipMaps = false;
	if (textureResourceCreateInfo.mipMap && imageViewResourceCreateInfo.mipLevels == 1)
	{
		imageViewResourceCreateInfo.mipLevels = (uint32_t)glm::max(log2f((float)textureResourceCreateInfo.imageDataResources.images[0].width), log2f((float)textureResourceCreateInfo.imageDataResources.images[0].height)) + 1;
		imageViewResourceCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

		creatMipMaps = true;
	}

	if (!Helper::createImageViewResource(physicalDevice, device, textureResource.imageViewResource, imageViewResourceCreateInfo))
	{
		return false;
	}

	//

	if (!transitionImageLayout(device, queue, commandPool, textureResource.imageViewResource.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, textureResourceCreateInfo.imageDataResources.mipLevels, textureResourceCreateInfo.imageDataResources.faceCount))
	{
		destroyImageViewResource(device, textureResource.imageViewResource);

		return false;
	}

	//

	for (const ImageDataResource& currentImageDataResource : textureResourceCreateInfo.imageDataResources.images)
	{
		BufferResourceCreateInfo stageBufferResourceCreateInfo = {};
		stageBufferResourceCreateInfo.size = currentImageDataResource.pixels.size();
		stageBufferResourceCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stageBufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		BufferResource stageBufferResource = {};
		if (!createBufferResource(physicalDevice, device, stageBufferResource, stageBufferResourceCreateInfo))
		{
			destroyImageViewResource(device, textureResource.imageViewResource);

			return false;
		}

		if (!copyHostToDevice(device, stageBufferResource, currentImageDataResource.pixels.data(), currentImageDataResource.pixels.size()))
		{
			destroyImageViewResource(device, textureResource.imageViewResource);

			destroyBufferResource(device, stageBufferResource);

			return false;
		}

		//

		if (!copyBufferToImage(device, queue, commandPool, stageBufferResource.buffer, textureResource.imageViewResource.image, currentImageDataResource.width, currentImageDataResource.height, currentImageDataResource.mipLevel, currentImageDataResource.face))
		{
			destroyImageViewResource(device, textureResource.imageViewResource);

			destroyBufferResource(device, stageBufferResource);

			return false;
		}

		destroyBufferResource(device, stageBufferResource);
	}

	//

	if (!transitionImageLayout(device, queue, commandPool, textureResource.imageViewResource.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, textureResourceCreateInfo.imageDataResources.mipLevels, textureResourceCreateInfo.imageDataResources.faceCount))
	{
		destroyImageViewResource(device, textureResource.imageViewResource);

		return false;
	}

	//

	if (creatMipMaps)
	{
		if (!generateMipMap(device, queue, commandPool, textureResource.imageViewResource.image, textureResourceCreateInfo.imageDataResources.images[0].width, textureResourceCreateInfo.imageDataResources.images[0].height, imageViewResourceCreateInfo.mipLevels, textureResourceCreateInfo.imageDataResources.faceCount))
		{
			destroyImageViewResource(device, textureResource.imageViewResource);

			return false;
		}
	}

	//

	SamplerResourceCreateInfo samplerResourceCreateInfo = textureResourceCreateInfo.samplerResourceCreateInfo;
	samplerResourceCreateInfo.maxLod = (float)imageViewResourceCreateInfo.mipLevels;

	if (!createSamplerResource(device, textureResource.samplerResource, samplerResourceCreateInfo))
	{
		destroyImageViewResource(device, textureResource.imageViewResource);

		return false;
	}

	return true;
}

bool Helper::createSamplerResource(VkDevice device, SamplerResource& samplerResource, const SamplerResourceCreateInfo& samplerResourceCreateInfo)
{
	VkResult result = VK_SUCCESS;

	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = samplerResourceCreateInfo.magFilter;
	samplerCreateInfo.minFilter = samplerResourceCreateInfo.minFilter;
	samplerCreateInfo.addressModeU = samplerResourceCreateInfo.addressModeU;
	samplerCreateInfo.addressModeV = samplerResourceCreateInfo.addressModeV;
	samplerCreateInfo.mipmapMode = samplerResourceCreateInfo.mipmapMode;
	samplerCreateInfo.minLod = samplerResourceCreateInfo.minLod;
	samplerCreateInfo.maxLod = samplerResourceCreateInfo.maxLod;

	result = vkCreateSampler(device, &samplerCreateInfo, nullptr, &samplerResource.sampler);
	if (result != VK_SUCCESS)
	{
		Logger::print(TE_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	return true;
}

void Helper::destroySamplerResource(VkDevice device, SamplerResource& samplerResource)
{
	if (samplerResource.sampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(device, samplerResource.sampler, nullptr);
		samplerResource.sampler = VK_NULL_HANDLE;
	}
}

void Helper::destroyTextureResource(VkDevice device, TextureResource& textureResource)
{
	destroySamplerResource(device, textureResource.samplerResource);

	destroyImageViewResource(device, textureResource.imageViewResource);
}

