#include "../render/HelperAccessResource.h"

VkBuffer HelperAccessResource::getBuffer(AllocationManager& allocationManager, const BufferView& bufferView)
{
	return allocationManager.getBufferViewResource(&bufferView)->vertexBufferResource.bufferResource.buffer;
}

VkBuffer HelperAccessResource::getBuffer(AllocationManager& allocationManager, const Accessor& accessor)
{
	if (accessor.aliasedBuffer.byteLength > 0)
	{
		return HelperAccessResource::getBuffer(allocationManager, accessor.aliasedBufferView);
	}

	if (accessor.sparse.count >= 1)
	{
		return HelperAccessResource::getBuffer(allocationManager, accessor.sparse.bufferView);
	}

	if (accessor.pBufferView == nullptr)
	{
		return VK_NULL_HANDLE;
	}

	return HelperAccessResource::getBuffer(allocationManager, *accessor.pBufferView);
}

VkImage HelperAccessResource::getImage(AllocationManager& allocationManager, const Texture& texture)
{
	return allocationManager.getTextureResource(&texture)->imageViewResource.image;
}

VkImageView HelperAccessResource::getImageView(AllocationManager& allocationManager, const Texture& texture)
{
	return allocationManager.getTextureResource(&texture)->imageViewResource.imageView;
}

VkSampler HelperAccessResource::getSampler(AllocationManager& allocationManager, const Texture& texture)
{
	return allocationManager.getTextureResource(&texture)->samplerResource.sampler;
}
