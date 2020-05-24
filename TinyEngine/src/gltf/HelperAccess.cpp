#include "HelperAccess.h"

const uint8_t* HelperAccess::accessData(const Buffer& buffer)
{
	return buffer.binary.data();
}

const uint8_t* HelperAccess::accessData(const BufferView& bufferView)
{
	return HelperAccess::accessData(*bufferView.pBuffer) + bufferView.byteOffset;
}

uint32_t HelperAccess::getOffset(const BufferView& bufferView)
{
	return bufferView.byteOffset;
}

const uint8_t* HelperAccess::accessData(const Accessor& accessor)
{
	if (accessor.aliasedBuffer.byteLength > 0)
	{
		return accessor.aliasedBuffer.binary.data() + accessor.byteOffset;
	}

	if (accessor.sparse.count >= 1)
	{
		return accessor.sparse.buffer.binary.data() + accessor.byteOffset;
	}

	return HelperAccess::accessData(*accessor.pBufferView) + accessor.byteOffset;
}

uint32_t HelperAccess::getOffset(const Accessor& accessor)
{
	return accessor.byteOffset;
}

uint32_t HelperAccess::getRange(const Accessor& accessor)
{
	if (accessor.aliasedBuffer.byteLength > 0)
	{
		return accessor.aliasedBufferView.byteLength - accessor.byteOffset;
	}

	if (accessor.sparse.count >= 1)
	{
		return accessor.sparse.bufferView.byteLength - accessor.byteOffset;
	}

	return accessor.pBufferView->byteLength - accessor.byteOffset;
}

uint32_t HelperAccess::getStride(const Accessor& accessor)
{
	uint32_t stride = 4;

	if (accessor.aliasedBuffer.byteLength > 0)
	{
		stride = accessor.aliasedBufferView.byteStride;
	}
	else if (accessor.sparse.count >= 1)
	{
		stride = accessor.sparse.bufferView.byteStride;
	}
	else if (accessor.pBufferView)
	{
		stride = accessor.pBufferView->byteStride;
	}

	return glm::max(accessor.typeCount * accessor.componentTypeSize, stride);
}

const uint8_t* HelperAccess::accessData(const Image& image, uint32_t index)
{
	if (index >= image.imageDataResources.images.size())
	{
		return nullptr;
	}

	return image.imageDataResources.images[index].pixels.data();
}
