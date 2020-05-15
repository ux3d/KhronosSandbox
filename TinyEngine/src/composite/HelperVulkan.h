#ifndef VULKAN_HELPERVULKAN_H_
#define VULKAN_HELPERVULKAN_H_

#include <cstdint>
#include <vector>

#include "../common/Common.h"

class HelperVulkan
{
public:

	static bool isDepthStencilFormat(VkFormat format);

	static bool getFormat(VkFormat& format, uint32_t componentTypeSize, bool componentTypeSigned, bool componentTypeInteger, uint32_t typeCount);

	static bool findMemoryTypeIndex(uint32_t& memoryTypeIndex, VkPhysicalDevice physicalDevice, uint32_t memoryType, VkMemoryPropertyFlags memoryProperty);

	static bool beginOneTimeSubmitCommand(VkDevice device, VkCommandPool commandPool, VkCommandBuffer& commandBuffer);

	static bool endOneTimeSubmitCommand(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkCommandBuffer commandBuffer);

	static bool transitionImageLayout(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t baseMipLevel, uint32_t levelCount, uint32_t layerCount);

	static bool copyBuffer(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	static bool copyBufferToImage(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkBuffer srcBuffer, VkImage dstImage, uint32_t width, uint32_t height, uint32_t mipLevel, uint32_t baseArrayLayer);

	static bool generateMipMap(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImage image, uint32_t width, uint32_t height, uint32_t levelCount, uint32_t layerCount);

};

#endif /* VULKAN_HELPERVULKAN_H_ */
