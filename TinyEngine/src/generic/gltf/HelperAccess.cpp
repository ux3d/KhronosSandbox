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

const uint8_t* HelperAccess::accessData(const Accessor& accessor)
{
	return HelperAccess::accessData(*accessor.pBufferView) + accessor.byteOffset;
}

size_t HelperAccess::getOffset(const Accessor& accessor)
{
	return accessor.byteOffset;
}

size_t HelperAccess::getRange(const Accessor& accessor)
{
	return accessor.pBufferView->byteLength - accessor.byteOffset;
}

const uint8_t* HelperAccess::accessData(const Image& image, uint32_t index)
{
	if (index >= image.imageDataResources.images.size())
	{
		return nullptr;
	}

	return image.imageDataResources.images[index].pixels.data();
}
