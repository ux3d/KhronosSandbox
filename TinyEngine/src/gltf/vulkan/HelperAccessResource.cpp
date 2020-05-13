#include "HelperAccessResource.h"

VkBuffer HelperAccessResource::getBuffer(ResourceManager& resourceManager, const BufferView& bufferView)
{
	return resourceManager.getBufferViewResource(&bufferView)->vertexBufferResource.bufferResource.buffer;
}

VkBuffer HelperAccessResource::getBuffer(ResourceManager& resourceManager, const Accessor& accessor)
{
	if (accessor.pBufferView == nullptr)
	{
		return VK_NULL_HANDLE;
	}

	return HelperAccessResource::getBuffer(resourceManager, *accessor.pBufferView);
}

VkImage HelperAccessResource::getImage(ResourceManager& resourceManager, const Texture& texture)
{
	return resourceManager.getTextureResource(&texture)->imageViewResource.image;
}

VkImageView HelperAccessResource::getImageView(ResourceManager& resourceManager, const Texture& texture)
{
	return resourceManager.getTextureResource(&texture)->imageViewResource.imageView;
}

VkSampler HelperAccessResource::getSampler(ResourceManager& resourceManager, const Texture& texture)
{
	return resourceManager.getTextureResource(&texture)->samplerResource.sampler;
}
