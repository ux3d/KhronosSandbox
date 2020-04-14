#include "HelperAccess.h"

const uint8_t* HelperAccess::accessData(const Buffer& buffer)
{
	return buffer.binary.data();
}

const uint8_t* HelperAccess::accessData(const BufferView& bufferView)
{
	return HelperAccess::accessData(*bufferView.pBuffer) + bufferView.byteOffset;
}

size_t HelperAccess::getOffset(const BufferView& bufferView)
{
	return bufferView.byteOffset;
}

VkBuffer HelperAccess::getBuffer(const BufferView& bufferView)
{
	return bufferView.vertexBufferResource.bufferResource.buffer;
}

const uint8_t* HelperAccess::accessData(const Accessor& accessor)
{
	return HelperAccess::accessData(*accessor.pBufferView) + accessor.byteOffset;
}

size_t HelperAccess::getOffset(const Accessor& accessor)
{
	return accessor.byteOffset;
}

VkBuffer HelperAccess::getBuffer(const Accessor& accessor)
{
	return HelperAccess::getBuffer(*accessor.pBufferView);
}

VkDeviceSize HelperAccess::getRange(const Accessor& accessor)
{
	return static_cast<VkDeviceSize>(accessor.pBufferView->byteLength - accessor.byteOffset);
}

const uint8_t* HelperAccess::accessData(const Image& image, uint32_t index)
{
	if (index >= image.imageDataResources.images.size())
	{
		return nullptr;
	}

	return image.imageDataResources.images[index].pixels.data();
}

std::string HelperAccess::getTexCoord(const TextureInfo& textureInfo)
{
	return "in_texCoord" + std::to_string(textureInfo.texCoord);
}
