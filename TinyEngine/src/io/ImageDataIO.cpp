#include "ImageDataIO.h"

#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_HDR
#include <stb_image.h>

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
		int x = 0;
		int y = 0;
		int comp = 0;
		int req_comp = static_cast<int>(channels);

		uint32_t bytesPerChannel = 1;
		bool isHDR = (bool)stbi_is_hdr_from_memory((const stbi_uc*)data, (int)length);

		uint8_t* tempData = nullptr;

		if (isHDR)
		{
			bytesPerChannel = 4;

			tempData = reinterpret_cast<uint8_t*>(stbi_loadf_from_memory((const stbi_uc*)data, (int)length, &x, &y, &comp, req_comp));
		}
		else
		{
			tempData = static_cast<uint8_t*>(stbi_load_from_memory((const stbi_uc*)data, (int)length, &x, &y, &comp, req_comp));
		}

		if (!tempData)
		{
			return false;
		}



		output.images[0].width = static_cast<uint32_t>(x);
		output.images[0].height = static_cast<uint32_t>(y);
		output.images[0].pixels.resize(output.images[0].width * output.images[0].height * channels * bytesPerChannel);
		memcpy(output.images[0].pixels.data(), tempData, output.images[0].width * output.images[0].height * channels * bytesPerChannel);
		switch (channels)
		{
			case 1:
				if (isHDR)
				{
					output.images[0].format = VK_FORMAT_R32_SFLOAT;
				}
				else
				{
					output.images[0].format = VK_FORMAT_R8_UNORM;
				}
				break;
			case 2:
				if (isHDR)
				{
					output.images[0].format = VK_FORMAT_R32G32_SFLOAT;
				}
				else
				{
					output.images[0].format = VK_FORMAT_R8G8_UNORM;
				}
				break;
			case 3:
				if (isHDR)
				{
					output.images[0].format = VK_FORMAT_R32G32B32_SFLOAT;
				}
				else
				{
					output.images[0].format = VK_FORMAT_R8G8B8_UNORM;
				}
				break;
			case 4:
				if (isHDR)
				{
					output.images[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				}
				else
				{
					output.images[0].format = VK_FORMAT_R8G8B8A8_UNORM;
				}
				break;
		}

		free(tempData);

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

	if (HelperFile::getExtension(filename) == "ktx2")
	{
		return open(output, (const uint8_t*)binary.data(), binary.length(), channels);
	}
	else if (HelperFile::getExtension(filename) == "png" || HelperFile::getExtension(filename) == "jpg" || HelperFile::getExtension(filename) == "jpeg" || HelperFile::getExtension(filename) == "hdr")
	{
		return open(output, (const uint8_t*)binary.data(), binary.length(), channels);
	}

	return false;
}
