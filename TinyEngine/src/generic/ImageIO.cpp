#include "ImageIO.h"

#include "../generic/FileIO.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include <stb_image.h>

#include "slimktx2.h"
#include "DefaultFileIOCallback.h"
#include "DefaultAllocationCallback.h"

using namespace ux3d;

bool ImageIO::openImageData(ImageDataResources& output, const std::string& filename, const uint32_t channels)
{
	if (channels < 1 || channels > 4)
	{
		return false;
	}

	if (FileIO::getExtension(filename) == "ktx2")
	{
		slimktx2::DefaultFileIOCallback defaultFileIOCallback;
		slimktx2::DefaultAllocationCallback defaultAllocationCallback;
		slimktx2::Callbacks callbacks = defaultFileIOCallback.getCallback();
		callbacks.allocate = defaultAllocationCallback.getCallback().allocate;
		callbacks.deallocate = defaultAllocationCallback.getCallback().deallocate;

		slimktx2::SlimKTX2 slimKTX2(callbacks);

		FILE* file = fopen(filename.c_str(), "rb");
		if (!file)
		{
			return false;
		}

		if (slimKTX2.parse((ux3d::slimktx2::IOHandle)file) != slimktx2::Result::Success)
		{
			fclose(file);

			return false;
		}
		fclose(file);

		if (slimktx2::isPacked(slimKTX2.getHeader().vkFormat) || slimktx2::isCompressed(slimKTX2.getHeader().vkFormat))
		{
			return false;
		}
		VkFormat format = (VkFormat)slimKTX2.getHeader().vkFormat;
		uint32_t pixelSize = slimktx2::getPixelSize(slimKTX2.getHeader().vkFormat);
		uint32_t width = slimKTX2.getHeader().pixelWidth;
		uint32_t height = slimKTX2.getHeader().pixelHeight;

		output.images.clear();
		output.mipLevels = slimKTX2.getLevelCount();
		output.faceCount = slimKTX2.getFaceCount();
		output.images.resize(output.mipLevels * output.faceCount);

		for (uint32_t level = 0; level < output.mipLevels; level++)
		{
			for (uint32_t face = 0; face < output.faceCount; face++)
			{
				uint32_t currentWidth = width >> level;
				uint32_t currentHeight = height >> level;

				output.images[face + output.faceCount * level].width = currentWidth;
				output.images[face + output.faceCount * level].height = currentHeight;
				output.images[face + output.faceCount * level].format = format;

				output.images[face + output.faceCount * level].mipLevel = level;
				output.images[face + output.faceCount * level].face = face;

				uint64_t imageSize = currentWidth * currentHeight * pixelSize;
				output.images[face + output.faceCount * level].pixels.resize(imageSize);

				uint8_t* outImageData = nullptr;
				if (slimKTX2.getImage(outImageData, level, face, 0) != slimktx2::Result::Success)
				{
					return false;
				}
				memcpy(output.images[face + output.faceCount * level].pixels.data(), outImageData, imageSize);
			}
		}

		return true;
	}
	else if (FileIO::getExtension(filename) == "png" || FileIO::getExtension(filename) == "jpg" || FileIO::getExtension(filename) == "jpeg")
	{
		int x = 0;
		int y = 0;
		int comp = 0;
		int req_comp = static_cast<int>(channels);

		uint8_t* tempData = static_cast<uint8_t*>(stbi_load(filename.c_str(), &x, &y, &comp, req_comp));
		if (!tempData)
		{
			return false;
		}

		output.images[0].width = static_cast<uint32_t>(x);
		output.images[0].height = static_cast<uint32_t>(y);
		output.images[0].pixels.resize(output.images[0].width * output.images[0].height * channels);
		memcpy(output.images[0].pixels.data(), tempData, output.images[0].width * output.images[0].height * channels);
		switch (channels)
		{
			case 1:
				output.images[0].format = VK_FORMAT_R8_UNORM;
				break;
			case 2:
				output.images[0].format = VK_FORMAT_R8G8_UNORM;
				break;
			case 3:
				output.images[0].format = VK_FORMAT_R8G8B8_UNORM;
				break;
			case 4:
				output.images[0].format = VK_FORMAT_R8G8B8A8_UNORM;
				break;
		}

		free(tempData);

		return true;
	}

	return false;
}
