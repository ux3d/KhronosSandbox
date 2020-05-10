#include "HelperAccessResource.h"

VkBuffer HelperAccessResource::getBuffer(ResourceManager& resourceManager, const BufferView* bufferView)
{
	if (bufferView == nullptr)
	{
		return VK_NULL_HANDLE;
	}

	return resourceManager.getBufferViewResource(bufferView)->vertexBufferResource.bufferResource.buffer;
}

VkBuffer HelperAccessResource::getBuffer(ResourceManager& resourceManager, const Accessor* accessor)
{
	if (accessor == nullptr)
	{
		return VK_NULL_HANDLE;
	}

	return HelperAccessResource::getBuffer(resourceManager, accessor->pBufferView);
}
