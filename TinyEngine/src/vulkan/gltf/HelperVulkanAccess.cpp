#include "HelperVulkanAccess.h"

VkBuffer HelperVulkanAccess::getBuffer(const BufferView& bufferView)
{
	return bufferView.vertexBufferResource.bufferResource.buffer;
}

VkBuffer HelperVulkanAccess::getBuffer(const Accessor& accessor)
{
	return HelperVulkanAccess::getBuffer(*accessor.pBufferView);
}
