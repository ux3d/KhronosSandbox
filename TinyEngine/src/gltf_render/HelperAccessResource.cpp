#include "HelperAccessResource.h"

VkBuffer HelperAccessResource::getBuffer(AllocationManager& resourceManager, const BufferView& bufferView)
{
	return resourceManager.getBufferViewResource(&bufferView)->vertexBufferResource.bufferResource.buffer;
}

VkBuffer HelperAccessResource::getBuffer(AllocationManager& resourceManager, const Accessor& accessor)
{
	if (accessor.aliasedBuffer.byteLength > 0)
	{
		return HelperAccessResource::getBuffer(resourceManager, accessor.aliasedBufferView);
	}

	if (accessor.sparse.count >= 1)
	{
		return HelperAccessResource::getBuffer(resourceManager, accessor.sparse.bufferView);
	}

	if (accessor.pBufferView == nullptr)
	{
		return VK_NULL_HANDLE;
	}

	return HelperAccessResource::getBuffer(resourceManager, *accessor.pBufferView);
}

VkImage HelperAccessResource::getImage(AllocationManager& resourceManager, const Texture& texture)
{
	return resourceManager.getTextureResource(&texture)->imageViewResource.image;
}

VkImageView HelperAccessResource::getImageView(AllocationManager& resourceManager, const Texture& texture)
{
	return resourceManager.getTextureResource(&texture)->imageViewResource.imageView;
}

VkSampler HelperAccessResource::getSampler(AllocationManager& resourceManager, const Texture& texture)
{
	return resourceManager.getTextureResource(&texture)->samplerResource.sampler;
}
