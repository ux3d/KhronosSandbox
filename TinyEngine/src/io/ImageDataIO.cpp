#include "ImageDataIO.h"

#include <cstdio>

#include <OpenImageIO/filesystem.h>
#include <OpenImageIO/imageio.h>
using namespace OIIO;

#include "slimktx2.h"
#include "DefaultAllocationCallback.h"
#include "DefaultMemoryStreamCallback.h"

#include "FileIO.h"
#include "HelperFile.h"

using namespace ux3d;

bool ImageDataIO::open(ImageDataResources& output, const uint8_t* data, size_t length, uint32_t channels)
{
	static uint8_t KTX2Identifier[12] = {
	  0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32, 0x30, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
	};

	if (!data || length < 12)
	{
		return false;
	}

	if (memcmp(data, KTX2Identifier, 12) == 0)
	{
		slimktx2::DefaultMemoryStream defaultMemoryStream(data, length);

		slimktx2::DefaultMemoryStreamCallback defaultMemoryStreamCallback;
		slimktx2::DefaultAllocationCallback defaultAllocationCallback;
		slimktx2::Callbacks callbacks = defaultMemoryStreamCallback.getCallback();
		callbacks.allocate = defaultAllocationCallback.getCallback().allocate;
		callbacks.deallocate = defaultAllocationCallback.getCallback().deallocate;

		slimktx2::SlimKTX2 slimKTX2(callbacks);

		if (slimKTX2.parse((ux3d::slimktx2::IOHandle)&defaultMemoryStream) != slimktx2::Result::Success)
		{
			return false;
		}

		if (slimktx2::isPacked(slimKTX2.getHeader().vkFormat) || slimktx2::isCompressed(slimKTX2.getHeader().vkFormat))
		{
			return false;
		}

		VkFormat format = (VkFormat)slimKTX2.getHeader().vkFormat;
		uint32_t pixelSize = slimktx2::getFormatSize(slimKTX2.getHeader().vkFormat);
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
	else
	{
		Filesystem::IOMemReader memReader((void*)data, length);

		auto imageInput = ImageInput::open("dummy", nullptr, &memReader);
		if (!imageInput)
		{
			return false;
		}

		uint32_t bytesPerChannel = 0;
		bool isHDR = false;

		const ImageSpec& imageSpec = imageInput->spec();
		auto newFormat = imageSpec.format;

		if (imageSpec.format == TypeDesc::UINT8)
		{
			bytesPerChannel = 1;
		}
		else if (imageSpec.format == TypeDesc::HALF)
		{
			bytesPerChannel = 2;
			isHDR = true;
		}
		else if (imageSpec.format == TypeDesc::FLOAT)
		{
			bytesPerChannel = 4;
			isHDR = true;
		}
		else
		{
			return false;
		}

		output.images[0].width = static_cast<uint32_t>(imageSpec.width);
		output.images[0].height = static_cast<uint32_t>(imageSpec.height);
		output.images[0].pixels.resize(output.images[0].width * output.images[0].height * channels * bytesPerChannel);

		imageInput->read_image(0, 0, 0, -1, newFormat, output.images[0].pixels.data(), channels * bytesPerChannel);

		switch (channels)
		{
			case 1:
				if (isHDR)
				{
					if (bytesPerChannel == 2)
					{
						output.images[0].format = VK_FORMAT_R16_SFLOAT;
					}
					else
					{
						output.images[0].format = VK_FORMAT_R32_SFLOAT;
					}
				}
				else
				{
					output.images[0].format = VK_FORMAT_R8_UNORM;
				}
				break;
			case 2:
				if (isHDR)
				{
					if (bytesPerChannel == 2)
					{
						output.images[0].format = VK_FORMAT_R16G16_SFLOAT;
					}
					else
					{
						output.images[0].format = VK_FORMAT_R32G32_SFLOAT;
					}
				}
				else
				{
					output.images[0].format = VK_FORMAT_R8G8_UNORM;
				}
				break;
			case 3:
				if (isHDR)
				{
					if (bytesPerChannel == 2)
					{
						output.images[0].format = VK_FORMAT_R16G16B16_SFLOAT;
					}
					else
					{
						output.images[0].format = VK_FORMAT_R32G32B32_SFLOAT;
					}
				}
				else
				{
					output.images[0].format = VK_FORMAT_R8G8B8_UNORM;
				}
				break;
			case 4:
				if (isHDR)
				{
					if (bytesPerChannel == 2)
					{
						output.images[0].format = VK_FORMAT_R16G16B16A16_SFLOAT;
					}
					else
					{
						output.images[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
					}
				}
				else
				{
					output.images[0].format = VK_FORMAT_R8G8B8A8_UNORM;
				}
				break;
		}

		return true;
	}

	return false;
}

bool ImageDataIO::open(ImageDataResources& output, const std::string& filename, uint32_t channels)
{
	if (channels < 1 || channels > 4)
	{
		return false;
	}

	std::string binary = "";
	if (!FileIO::open(binary, filename))
	{
		return false;
	}

	if (HelperFile::getExtension(filename) == "ktx2" || HelperFile::getExtension(filename) == "png" || HelperFile::getExtension(filename) == "jpg" || HelperFile::getExtension(filename) == "jpeg" || HelperFile::getExtension(filename) == "hdr" || HelperFile::getExtension(filename) == "exr")
	{
		return open(output, (const uint8_t*)binary.data(), binary.length(), channels);
	}

	return false;
}
