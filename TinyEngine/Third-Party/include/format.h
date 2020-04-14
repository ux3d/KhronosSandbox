// Copyright (c) 2020 UX3D GmbH. All rights reserved.

#pragma once

#include <cstdint>

namespace ux3d
{
	namespace slimktx2
	{
		enum Channel : uint32_t
		{
			Channel_Red = 0,
			Channel_Green,
			Channel_Blue,
			Channel_Alpha
		};

		// TODO: rename to PixelFormat
		enum class Format : uint32_t
		{
			UNDEFINED = 0u,

			//
			// PACKED
			//

			R4G4_UNORM_PACK8 = 1,
			R4G4B4A4_UNORM_PACK16 = 2,
			B4G4R4A4_UNORM_PACK16 = 3,
			R5G6B5_UNORM_PACK16 = 4,
			B5G6R5_UNORM_PACK16 = 5,
			R5G5B5A1_UNORM_PACK16 = 6,
			B5G5R5A1_UNORM_PACK16 = 7,
			A1R5G5B5_UNORM_PACK16 = 8,

			A2R10G10B10_UNORM_PACK32 = 58,
			A2R10G10B10_SNORM_PACK32 = 59,

			A2R10G10B10_UINT_PACK32 = 62,
			A2R10G10B10_SINT_PACK32 = 63,
			A2B10G10R10_UNORM_PACK32 = 64,
			A2B10G10R10_SNORM_PACK32 = 65,

			A2B10G10R10_UINT_PACK32 = 68,
			A2B10G10R10_SINT_PACK32 = 69,

			B10G11R11_UFLOAT_PACK32 = 122,

			///
			/// 8 BIT PER CHANNEL
			///

			// R8
			R8_UNORM = 9,
			R8_SNORM = 10,
			R8_UINT = 13,
			R8_SINT = 14,
			R8_SRGB = 15,

			// RG8
			R8G8_UNORM = 16,
			R8G8_SNORM = 17,
			R8G8_UINT = 20,
			R8G8_SINT = 21,
			R8G8_SRGB = 22,

			// RGB8
			R8G8B8_UNORM = 23,
			R8G8B8_SNORM = 24,
			R8G8B8_UINT = 27,
			R8G8B8_SINT = 28,
			R8G8B8_SRGB = 29,

			// BGR8
			B8G8R8_UNORM = 30,
			B8G8R8_SNORM = 31,
			B8G8R8_UINT = 34,
			B8G8R8_SINT = 35,
			B8G8R8_SRGB = 36,

			// RGBA8
			R8G8B8A8_UNORM = 37,
			R8G8B8A8_SNORM = 38,
			R8G8B8A8_UINT = 41,
			R8G8B8A8_SINT = 42,
			R8G8B8A8_SRGB = 43,

			// BGRA8
			B8G8R8A8_UNORM = 44,
			B8G8R8A8_SNORM = 45,
			B8G8R8A8_UINT = 48,
			B8G8R8A8_SINT = 49,
			B8G8R8A8_SRGB = 50,

			///
			/// 16 BIT PER CHANNEL
			///

			// R16
			R16_UNORM = 70,
			R16_SNORM = 71,
			R16_UINT = 74,
			R16_SINT = 75,
			R16_SFLOAT = 76,

			// RG16
			R16G16_UNORM = 77,
			R16G16_SNORM = 78,
			R16G16_UINT = 81,
			R16G16_SINT = 82,
			R16G16_SFLOAT = 83,

			// RGB16
			R16G16B16_UNORM = 84,
			R16G16B16_SNORM = 85,
			R16G16B16_UINT = 88,
			R16G16B16_SINT = 89,
			R16G16B16_SFLOAT = 90,

			// RGBA16
			R16G16B16A16_UNORM = 91,
			R16G16B16A16_SNORM = 92,
			R16G16B16A16_UINT = 95,
			R16G16B16A16_SINT = 96,
			R16G16B16A16_SFLOAT = 97,

			///
			/// 32 BIT PER CHANNEL
			///

			// R32
			R32_UINT = 98,
			R32_SINT = 99,
			R32_SFLOAT = 100,

			// RG32
			R32G32_UINT = 101,
			R32G32_SINT = 102,
			R32G32_SFLOAT = 103,

			// RGB32
			R32G32B32_UINT = 104,
			R32G32B32_SINT = 105,
			R32G32B32_SFLOAT = 106,

			// RGBA32
			R32G32B32A32_UINT = 107,
			R32G32B32A32_SINT = 108,
			R32G32B32A32_SFLOAT = 109,

			///
			/// 64 BIT PER CHANNEL
			///

			R64_UINT = 110,
			R64_SINT = 111,
			R64_SFLOAT = 112,
			R64G64_UINT = 113,
			R64G64_SINT = 114,
			R64G64_SFLOAT = 115,
			R64G64B64_UINT = 116,
			R64G64B64_SINT = 117,
			R64G64B64_SFLOAT = 118,
			R64G64B64A64_UINT = 119,
			R64G64B64A64_SINT = 120,
			R64G64B64A64_SFLOAT = 121
		};

		// as defined by KTX (channel size)
		uint32_t getTypeSize(Format _vkFormat);

		// as defined by vulkan (pixel size)
		uint32_t getPixelSize(Format _vkFormat);

		uint32_t getChannelCount(Format _vkFormat);

		uint32_t getChannelSize(Format _vkFormat, uint32_t _channelIndex);

		int32_t getChannelIndex(Format _vkFormat, Channel _channel);

		bool isFloat(Format _vkFormat);

		bool isSigned(Format _vkFormat);

		bool isNormalized(Format _vkFormat);

		bool isSrgb(Format _vkFormat);

		bool isPacked(Format _vkFormat);

		bool isCompressed(Format _vkFormat);

		uint64_t getFaceSize(uint32_t _pixelByteSize, uint32_t _level, uint32_t _width, uint32_t _height, uint32_t _depth = 0u);

		// computes the pixel count (resolution) of an image of the given level
		uint32_t getPixelCount(uint32_t _level, uint32_t _width, uint32_t _height, uint32_t _depth);

		uint32_t getPadding(uint64_t _value, uint32_t _alginment);

		// greedy least common multiple
		uint32_t getLCM(uint32_t _x, uint32_t _y);

		// _value / offset to apply lcm4 padding to (if applicable)
		uint32_t getMipPadding(uint64_t _value, Format _vkFormat, bool _superCompression);
	}// !slimktx2
} // ux3d
