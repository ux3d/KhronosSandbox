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
	return accessor.pBufferView->byteLength - accessor.byteOffset;
}

uint32_t HelperAccess::getStride(const Accessor& accessor)
{
	return accessor.typeCount * accessor.componentTypeSize;
}

const uint8_t* HelperAccess::accessData(const Image& image, uint32_t index)
{
	if (index >= image.imageDataResources.images.size())
	{
		return nullptr;
	}

	return image.imageDataResources.images[index].pixels.data();
}
