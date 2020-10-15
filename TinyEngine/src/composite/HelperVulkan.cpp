#include "HelperVulkan.h"

#include "../math/Math.h"

bool HelperVulkan::isDepthStencilFormat(VkFormat format)
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

bool HelperVulkan::getFormat(VkFormat& format, uint32_t componentTypeSize, bool componentTypeSigned, bool componentTypeInteger, uint32_t typeCount, bool normalized)
{
	bool result = false;
	format = VK_FORMAT_UNDEFINED;

	if (componentTypeInteger)
	{
		if (componentTypeSize == 1)
		{
			switch (typeCount)
			{
				case 1:
					if (normalized)
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R8_SNORM;
						}
						else
						{
							format = VK_FORMAT_R8_UNORM;
						}
					}
					else
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R8_SINT;
						}
						else
						{
							format = VK_FORMAT_R8_UINT;
						}
					}
					result = true;
					break;
				case 2:
					if (normalized)
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R8G8_SNORM;
						}
						else
						{
							format = VK_FORMAT_R8G8_UNORM;
						}
					}
					else
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R8G8_SINT;
						}
						else
						{
							format = VK_FORMAT_R8G8_UINT;
						}
					}
					result = true;
					break;
				case 3:
					if (normalized)
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R8G8B8_SNORM;
						}
						else
						{
							format = VK_FORMAT_R8G8B8_UNORM;
						}
					}
					else
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R8G8B8_SINT;
						}
						else
						{
							format = VK_FORMAT_R8G8B8_UINT;
						}
					}
					result = true;
					break;
				case 4:
					if (normalized)
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R8G8B8A8_SNORM;
						}
						else
						{
							format = VK_FORMAT_R8G8B8A8_UNORM;
						}
					}
					else
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R8G8B8A8_SINT;
						}
						else
						{
							format = VK_FORMAT_R8G8B8A8_UINT;
						}
					}
					result = true;
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
					if (normalized)
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R16_SNORM;
						}
						else
						{
							format = VK_FORMAT_R16_UNORM;
						}
					}
					else
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R16_SINT;
						}
						else
						{
							format = VK_FORMAT_R16_UINT;
						}
					}
					result = true;
					break;
				case 2:
					if (normalized)
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R16G16_SNORM;
						}
						else
						{
							format = VK_FORMAT_R16G16_UNORM;
						}
					}
					else
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R16G16_SINT;
						}
						else
						{
							format = VK_FORMAT_R16G16_UINT;
						}
					}
					result = true;
					break;
				case 3:
					if (normalized)
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R16G16B16_SNORM;
						}
						else
						{
							format = VK_FORMAT_R16G16B16_UNORM;
						}
					}
					else
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R16G16B16_SINT;
						}
						else
						{
							format = VK_FORMAT_R16G16B16_UINT;
						}
					}
					result = true;
					break;
				case 4:
					if (normalized)
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R16G16B16A16_SNORM;
						}
						else
						{
							format = VK_FORMAT_R16G16B16A16_UNORM;
						}
					}
					else
					{
						if (componentTypeSigned)
						{
							format = VK_FORMAT_R16G16B16A16_SINT;
						}
						else
						{
							format = VK_FORMAT_R16G16B16A16_UINT;
						}
					}
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
					if (componentTypeSigned)
					{
						format = VK_FORMAT_R32_SINT;
					}
					else
					{
						format = VK_FORMAT_R32_UINT;
					}
					result = true;
					break;
				case 2:
					if (componentTypeSigned)
					{
						format = VK_FORMAT_R32G32_SINT;
					}
					else
					{
						format = VK_FORMAT_R32G32_UINT;
					}
					result = true;
					break;
				case 3:
					if (componentTypeSigned)
					{
						format = VK_FORMAT_R32G32B32_SINT;
					}
					else
					{
						format = VK_FORMAT_R32G32B32_UINT;
					}
					result = true;
					break;
				case 4:
					if (componentTypeSigned)
					{
						format = VK_FORMAT_R32G32B32A32_SINT;
					}
					else
					{
						format = VK_FORMAT_R32G32B32A32_UINT;
					}
					result = true;
					break;
				default:
					break;
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

bool HelperVulkan::getTypeCount(uint32_t& typeCount, VkFormat format)
{
	switch (format)
	{
		case VK_FORMAT_R8_SNORM:
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8_UINT:
		case VK_FORMAT_R16_SNORM:
		case VK_FORMAT_R16_UNORM:
		case VK_FORMAT_R16_SINT:
		case VK_FORMAT_R16_UINT:
		case VK_FORMAT_R32_SINT:
		case VK_FORMAT_R32_UINT:
		case VK_FORMAT_R16_SFLOAT:
		case VK_FORMAT_R32_SFLOAT:
			typeCount = 1;
			return true;

		case VK_FORMAT_R8G8_SNORM:
		case VK_FORMAT_R8G8_UNORM:
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R8G8_UINT:
		case VK_FORMAT_R16G16_SNORM:
		case VK_FORMAT_R16G16_UNORM:
		case VK_FORMAT_R16G16_SINT:
		case VK_FORMAT_R16G16_UINT:
		case VK_FORMAT_R32G32_SINT:
		case VK_FORMAT_R32G32_UINT:
		case VK_FORMAT_R16G16_SFLOAT:
		case VK_FORMAT_R32G32_SFLOAT:
			typeCount = 2;
			return true;

		case VK_FORMAT_R8G8B8_SNORM:
		case VK_FORMAT_R8G8B8_UNORM:
		case VK_FORMAT_R8G8B8_SINT:
		case VK_FORMAT_R8G8B8_UINT:
		case VK_FORMAT_R16G16B16_SNORM:
		case VK_FORMAT_R16G16B16_UNORM:
		case VK_FORMAT_R16G16B16_SINT:
		case VK_FORMAT_R16G16B16_UINT:
		case VK_FORMAT_R32G32B32_SINT:
		case VK_FORMAT_R32G32B32_UINT:
		case VK_FORMAT_R16G16B16_SFLOAT:
		case VK_FORMAT_R32G32B32_SFLOAT:
			typeCount = 3;
			return true;

		case VK_FORMAT_R8G8B8A8_SNORM:
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_R16G16B16A16_SNORM:
		case VK_FORMAT_R16G16B16A16_UNORM:
		case VK_FORMAT_R16G16B16A16_SINT:
		case VK_FORMAT_R16G16B16A16_UINT:
		case VK_FORMAT_R32G32B32A32_SINT:
		case VK_FORMAT_R32G32B32A32_UINT:
		case VK_FORMAT_R16G16B16A16_SFLOAT:
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			typeCount = 4;
			return true;
		default:
			break;
	}

	return false;
}

bool HelperVulkan::getComponentTypeSize(uint32_t& componentTypeSize, VkFormat format)
{
	switch (format)
	{
		case VK_FORMAT_R8_SNORM:
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8_UINT:
		case VK_FORMAT_R8G8_SNORM:
		case VK_FORMAT_R8G8_UNORM:
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R8G8_UINT:
		case VK_FORMAT_R8G8B8_SNORM:
		case VK_FORMAT_R8G8B8_UNORM:
		case VK_FORMAT_R8G8B8_SINT:
		case VK_FORMAT_R8G8B8_UINT:
		case VK_FORMAT_R8G8B8A8_SNORM:
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_R8G8B8A8_UINT:
			componentTypeSize = 1;
			return true;

		case VK_FORMAT_R16_SNORM:
		case VK_FORMAT_R16_UNORM:
		case VK_FORMAT_R16_SINT:
		case VK_FORMAT_R16_UINT:
		case VK_FORMAT_R16_SFLOAT:
		case VK_FORMAT_R16G16_SNORM:
		case VK_FORMAT_R16G16_UNORM:
		case VK_FORMAT_R16G16_SINT:
		case VK_FORMAT_R16G16_UINT:
		case VK_FORMAT_R16G16B16_SNORM:
		case VK_FORMAT_R16G16B16_UNORM:
		case VK_FORMAT_R16G16B16_SINT:
		case VK_FORMAT_R16G16B16_UINT:
		case VK_FORMAT_R16G16B16_SFLOAT:
		case VK_FORMAT_R16G16B16A16_SNORM:
		case VK_FORMAT_R16G16B16A16_UNORM:
		case VK_FORMAT_R16G16B16A16_SINT:
		case VK_FORMAT_R16G16B16A16_UINT:
		case VK_FORMAT_R16G16B16A16_SFLOAT:
			componentTypeSize = 2;
			return true;

		case VK_FORMAT_R32_SINT:
		case VK_FORMAT_R32_UINT:
		case VK_FORMAT_R32_SFLOAT:
		case VK_FORMAT_R32G32_SINT:
		case VK_FORMAT_R32G32_UINT:
		case VK_FORMAT_R16G16_SFLOAT:
		case VK_FORMAT_R32G32_SFLOAT:
		case VK_FORMAT_R32G32B32_SINT:
		case VK_FORMAT_R32G32B32_UINT:
		case VK_FORMAT_R32G32B32_SFLOAT:
		case VK_FORMAT_R32G32B32A32_SINT:
		case VK_FORMAT_R32G32B32A32_UINT:
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			componentTypeSize = 4;
			return true;
		default:
			break;
	}

	return false;
}

bool HelperVulkan::getAligenedSize(VkDeviceSize& alignedSize, VkDeviceSize unalignedSize, VkDeviceSize alignment)
{
	if (alignment <= 1)
	{
		alignedSize = unalignedSize;

		return true;
	}

	VkDeviceSize factor = unalignedSize / alignment;
	VkDeviceSize remainer = unalignedSize % alignment;
	if (remainer != 0)
	{
		factor++;
	}

	alignedSize = alignment * factor;

	return true;
}

bool HelperVulkan::findMemoryTypeIndex(uint32_t& memoryTypeIndex, VkPhysicalDevice physicalDevice, uint32_t memoryType, VkMemoryPropertyFlags memoryProperty)
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

bool HelperVulkan::beginOneTimeSubmitCommand(VkDevice device, VkCommandPool commandPool, VkCommandBuffer& commandBuffer)
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
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

		return false;
	}

	return true;
}

bool HelperVulkan::endOneTimeSubmitCommand(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkCommandBuffer commandBuffer)
{
	VkResult result = VK_SUCCESS;

	result = vkEndCommandBuffer(commandBuffer);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

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
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

		return false;
	}

	result = vkQueueWaitIdle(queue);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

		return false;
	}

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

	return true;
}

bool HelperVulkan::transitionImageLayout(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t baseMipLevel, uint32_t levelCount, uint32_t layerCount)
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

bool HelperVulkan::copyBuffer(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
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

bool HelperVulkan::copyBufferToImage(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkBuffer srcBuffer, VkImage dstImage, uint32_t width, uint32_t height, uint32_t mipLevel, uint32_t baseArrayLayer)
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

bool HelperVulkan::generateMipMap(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImage image, uint32_t width, uint32_t height, uint32_t levelCount, uint32_t layerCount)
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
