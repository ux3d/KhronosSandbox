#include "../render/HelperAccessResource.h"

VkBuffer HelperAccessResource::getBuffer(ResourceManager& resourceManager, const BufferView& bufferView)
{
	return resourceManager.getBufferViewResource((uint64_t)&bufferView)->vertexBufferResource.bufferResource.buffer;
}

VkBuffer HelperAccessResource::getBuffer(ResourceManager& resourceManager, const Accessor& accessor)
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

VkImage HelperAccessResource::getImage(ResourceManager& resourceManager, const Texture& texture)
{
	return resourceManager.getTextureResource((uint64_t)&texture)->imageViewResource.image;
}

VkImageView HelperAccessResource::getImageView(ResourceManager& resourceManager, const Texture& texture)
{
	return resourceManager.getTextureResource((uint64_t)&texture)->imageViewResource.imageView;
}

VkSampler HelperAccessResource::getSampler(ResourceManager& resourceManager, const Texture& texture)
{
	return resourceManager.getTextureResource((uint64_t)&texture)->samplerResource.sampler;
}
