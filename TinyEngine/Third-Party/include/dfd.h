// Copyright (c) 2020 UX3D GmbH. All rights reserved.

#pragma once

#include <cstdint>
#include <cstddef>

namespace ux3d
{
	namespace slimktx2
	{
		enum VendorId : uint32_t // 17 bit
		{
			VendorId_KHRONOS = 0U,
			VendorId_MAX = 0x1FFFFU
		};

		enum DescriptorType : uint16_t // 15 bit
		{
			DescriptorType_BASICFORMAT = 0U,
			DescriptorType_ADDITIONAL_PLANES = 0x6001U,
			DescriptorType_ADDITIONAL_DIMENSIONS = 0x6002U,
			DescriptorType_NEEDED_FOR_WRITE_BIT = 0x2000U,
			DescriptorType_NEEDED_FOR_DECODE_BIT = 0x4000U,
			DescriptorType_MAX = 0x7FFFU
		};

		enum VersionNumber : uint16_t // 16 bit
		{
			VersionNumber_1_0 = 0U,
			VersionNumber_1_1 = 0U,
			VersionNumber_1_2 = 1U,
			VersionNumber_1_3 = 2U,
			VersionNumber_LATEST = VersionNumber_1_3,
			VersionNumber_MAX = 0xFFFFU
		};

		enum ColorModel : uint8_t
		{
            ColorModel_UNSPECIFIED = 0U,

			ColorModel_RGBSDA = 1U,
			ColorModel_YUVSDA = 2U,
			ColorModel_YIQSDA = 3U,
			ColorModel_LABSDA = 4U,
			ColorModel_CMYKA = 5U,
			ColorModel_XYZW = 6U,
			ColorModel_HSVA_ANG = 7U,
			ColorModel_HSLA_ANG = 8U,
			ColorModel_HSVA_HEX = 9U,
			ColorModel_HSLA_HEX = 10U,
			ColorModel_YCGCOA = 11U,
			ColorModel_YCCBCCRC = 12U,
			ColorModel_ICTCP = 13U,
			ColorModel_CIEXYZ = 14U,
			ColorModel_CIEXYY = 15U,

			ColorModel_DXT1A = 128U,
			ColorModel_BC1A = 128U,
			ColorModel_DXT2 = 129U,
			ColorModel_DXT3 = 129U,
			ColorModel_BC2 = 129U,
			ColorModel_DXT4 = 130U,
			ColorModel_DXT5 = 130U,
			ColorModel_BC3 = 130U,
			ColorModel_BC4 = 131U,
			ColorModel_BC5 = 132U,
			ColorModel_BC6H = 133U,
			ColorModel_BC7 = 134U,

            ColorModel_ETC1 = 160U,

			ColorModel_ETC2 = 161U,
			ColorModel_ASTC = 162U,
			ColorModel_ETC1S = 163U,
			ColorModel_PVRTC = 164U,
			ColorModel_PVRTC2 = 165U,
			ColorModel_MAX = 0xFFU
		};

        enum ColorChannels : uint8_t
        {
            ColorChannels_UNSPECIFIED_0 = 0U,
            ColorChannels_UNSPECIFIED_1 = 1U,
            ColorChannels_UNSPECIFIED_2 = 2U,
            ColorChannels_UNSPECIFIED_3 = 3U,
            ColorChannels_UNSPECIFIED_4 = 4U,
            ColorChannels_UNSPECIFIED_5 = 5U,
            ColorChannels_UNSPECIFIED_6 = 6U,
            ColorChannels_UNSPECIFIED_7 = 7U,
            ColorChannels_UNSPECIFIED_8 = 8U,
            ColorChannels_UNSPECIFIED_9 = 9U,
            ColorChannels_UNSPECIFIED_10 = 10U,
            ColorChannels_UNSPECIFIED_11 = 11U,
            ColorChannels_UNSPECIFIED_12 = 12U,
            ColorChannels_UNSPECIFIED_13 = 13U,
            ColorChannels_UNSPECIFIED_14 = 14U,
            ColorChannels_UNSPECIFIED_15 = 15U,

            ColorChannels_RGBSDA_RED = 0U,
            ColorChannels_RGBSDA_R = 0U,
            ColorChannels_RGBSDA_GREEN = 1U,
            ColorChannels_RGBSDA_G = 1U,
            ColorChannels_RGBSDA_BLUE = 2U,
            ColorChannels_RGBSDA_B = 2U,
            ColorChannels_RGBSDA_STENCIL = 13U,
            ColorChannels_RGBSDA_S = 13U,
            ColorChannels_RGBSDA_DEPTH = 14U,
            ColorChannels_RGBSDA_D = 14U,
            ColorChannels_RGBSDA_ALPHA = 15U,
            ColorChannels_RGBSDA_A = 15U,

            ColorChannels_YUVSDA_Y = 0U,
            ColorChannels_YUVSDA_CB = 1U,
            ColorChannels_YUVSDA_U = 1U,
            ColorChannels_YUVSDA_CR = 2U,
            ColorChannels_YUVSDA_V = 2U,
            ColorChannels_YUVSDA_STENCIL = 13U,
            ColorChannels_YUVSDA_S = 13U,
            ColorChannels_YUVSDA_DEPTH = 14U,
            ColorChannels_YUVSDA_D = 14U,
            ColorChannels_YUVSDA_ALPHA = 15U,
            ColorChannels_YUVSDA_A = 15U,

            ColorChannels_YIQSDA_Y = 0U,
            ColorChannels_YIQSDA_I = 1U,
            ColorChannels_YIQSDA_Q = 2U,
            ColorChannels_YIQSDA_STENCIL = 13U,
            ColorChannels_YIQSDA_S = 13U,
            ColorChannels_YIQSDA_DEPTH = 14U,
            ColorChannels_YIQSDA_D = 14U,
            ColorChannels_YIQSDA_ALPHA = 15U,
            ColorChannels_YIQSDA_A = 15U,

            ColorChannels_LABSDA_L = 0U,
            ColorChannels_LABSDA_A = 1U,
            ColorChannels_LABSDA_B = 2U,
            ColorChannels_LABSDA_STENCIL = 13U,
            ColorChannels_LABSDA_S = 13U,
            ColorChannels_LABSDA_DEPTH = 14U,
            ColorChannels_LABSDA_D = 14U,
            ColorChannels_LABSDA_ALPHA = 15U,

            ColorChannels_CMYKSDA_CYAN = 0U,
            ColorChannels_CMYKSDA_C = 0U,
            ColorChannels_CMYKSDA_MAGENTA = 1U,
            ColorChannels_CMYKSDA_M = 1U,
            ColorChannels_CMYKSDA_YELLOW = 2U,
            ColorChannels_CMYKSDA_Y = 2U,
            ColorChannels_CMYKSDA_KEY = 3U,
            ColorChannels_CMYKSDA_BLACK = 3U,
            ColorChannels_CMYKSDA_K = 3U,
            ColorChannels_CMYKSDA_ALPHA = 15U,
            ColorChannels_CMYKSDA_A = 15U,

            ColorChannels_XYZW_X = 0U,
            ColorChannels_XYZW_Y = 1U,
            ColorChannels_XYZW_Z = 2U,
            ColorChannels_XYZW_W = 3U,

            ColorChannels_HSVA_ANG_VALUE = 0U,
            ColorChannels_HSVA_ANG_V = 0U,
            ColorChannels_HSVA_ANG_SATURATION = 1U,
            ColorChannels_HSVA_ANG_S = 1U,
            ColorChannels_HSVA_ANG_HUE = 2U,
            ColorChannels_HSVA_ANG_H = 2U,
            ColorChannels_HSVA_ANG_ALPHA = 15U,
            ColorChannels_HSVA_ANG_A = 15U,

            ColorChannels_HSLA_ANG_LIGHTNESS = 0U,
            ColorChannels_HSLA_ANG_L = 0U,
            ColorChannels_HSLA_ANG_SATURATION = 1U,
            ColorChannels_HSLA_ANG_S = 1U,
            ColorChannels_HSLA_ANG_HUE = 2U,
            ColorChannels_HSLA_ANG_H = 2U,
            ColorChannels_HSLA_ANG_ALPHA = 15U,
            ColorChannels_HSLA_ANG_A = 15U,

            ColorChannels_HSVA_HEX_VALUE = 0U,
            ColorChannels_HSVA_HEX_V = 0U,
            ColorChannels_HSVA_HEX_SATURATION = 1U,
            ColorChannels_HSVA_HEX_S = 1U,
            ColorChannels_HSVA_HEX_HUE = 2U,
            ColorChannels_HSVA_HEX_H = 2U,
            ColorChannels_HSVA_HEX_ALPHA = 15U,
            ColorChannels_HSVA_HEX_A = 15U,

            ColorChannels_HSLA_HEX_LIGHTNESS = 0U,
            ColorChannels_HSLA_HEX_L = 0U,
            ColorChannels_HSLA_HEX_SATURATION = 1U,
            ColorChannels_HSLA_HEX_S = 1U,
            ColorChannels_HSLA_HEX_HUE = 2U,
            ColorChannels_HSLA_HEX_H = 2U,
            ColorChannels_HSLA_HEX_ALPHA = 15U,
            ColorChannels_HSLA_HEX_A = 15U,

            ColorChannels_YCGCOA_Y = 0U,
            ColorChannels_YCGCOA_CG = 1U,
            ColorChannels_YCGCOA_CO = 2U,
            ColorChannels_YCGCOA_ALPHA = 15U,
            ColorChannels_YCGCOA_A = 15U,

            ColorChannels_CIEXYZ_X = 0U,
            ColorChannels_CIEXYZ_Y = 1U,
            ColorChannels_CIEXYZ_Z = 2U,

            ColorChannels_CIEXYY_X = 0U,
            ColorChannels_CIEXYY_YCHROMA = 1U,
            ColorChannels_CIEXYY_YLUMA = 2U,

            ColorChannels_DXT1A_COLOR = 0U,
            ColorChannels_BC1A_COLOR = 0U,
            ColorChannels_DXT1A_ALPHAPRESENT = 1U,
            ColorChannels_DXT1A_ALPHA = 1U,
            ColorChannels_BC1A_ALPHAPRESENT = 1U,
            ColorChannels_BC1A_ALPHA = 1U,

            ColorChannels_DXT2_COLOR = 0U,
            ColorChannels_DXT3_COLOR = 0U,
            ColorChannels_BC2_COLOR = 0U,
            ColorChannels_DXT2_ALPHA = 15U,
            ColorChannels_DXT3_ALPHA = 15U,
            ColorChannels_BC2_ALPHA = 15U,

            ColorChannels_DXT4_COLOR = 0U,
            ColorChannels_DXT5_COLOR = 0U,
            ColorChannels_BC3_COLOR = 0U,
            ColorChannels_DXT4_ALPHA = 15U,
            ColorChannels_DXT5_ALPHA = 15U,
            ColorChannels_BC3_ALPHA = 15U,

            ColorChannels_BC4_DATA = 0U,

            ColorChannels_BC5_RED = 0U,
            ColorChannels_BC5_R = 0U,
            ColorChannels_BC5_GREEN = 1U,
            ColorChannels_BC5_G = 1U,

            ColorChannels_BC6H_COLOR = 0U,
            ColorChannels_BC6H_DATA = 0U,

            ColorChannels_BC7_DATA = 0U,
            ColorChannels_BC7_COLOR = 0U,

            ColorChannels_ETC1_DATA = 0U,
            ColorChannels_ETC1_COLOR = 0U,

            ColorChannels_ETC2_RED = 0U,
            ColorChannels_ETC2_R = 0U,
            ColorChannels_ETC2_GREEN = 1U,
            ColorChannels_ETC2_G = 1U,
            ColorChannels_ETC2_COLOR = 2U,
            ColorChannels_ETC2_ALPHA = 15U,
            ColorChannels_ETC2_A = 15U,

            ColorChannels_ASTC_DATA = 0U,

            ColorChannels_ETC1S_DATA = 0U,
            ColorChannels_ETC1S_COLOR = 0U,

            ColorChannels_PVRTC_DATA = 0U,
            ColorChannels_PVRTC_COLOR = 0U,

            ColorChannels_PVRTC2_DATA = 0U,
            ColorChannels_PVRTC2_COLOR = 0U,

            ColorChannels_COMMON_LUMA = 0U,
            ColorChannels_COMMON_L = 0U,
            ColorChannels_COMMON_STENCIL = 13U,
            ColorChannels_COMMON_S = 13U,
            ColorChannels_COMMON_DEPTH = 14U,
            ColorChannels_COMMON_D = 14U,
            ColorChannels_COMMON_ALPHA = 15U,
            ColorChannels_COMMON_A = 15U
        };

        enum ColorPrimaries : uint8_t
        {
            ColorPrimaries_UNSPECIFIED = 0U,
            ColorPrimaries_BT709 = 1U,
            ColorPrimaries_SRGB = 1U,
            ColorPrimaries_BT601_EBU = 2U,
            ColorPrimaries_BT601_SMPTE = 3U,
            ColorPrimaries_BT2020 = 4U,
            ColorPrimaries_CIEXYZ = 5U,
            ColorPrimaries_ACES = 6U,
            ColorPrimaries_ACESCC = 7U,
            ColorPrimaries_NTSC1953 = 8U,
            ColorPrimaries_PAL525 = 9U,
            ColorPrimaries_DISPLAYP3 = 10U,
            ColorPrimaries_ADOBERGB = 11U,
            ColorPrimaries_MAX = 0xFFU
        };

        enum TransferFunction : uint8_t
        {
            TransferFunction_UNSPECIFIED = 0U,
            TransferFunction_LINEAR = 1U,
            TransferFunction_SRGB = 2U,
            TransferFunction_ITU = 3U,
            TransferFunction_SMTPE170M = 3U,
            TransferFunction_NTSC = 4U,
            TransferFunction_SLOG = 5U,
            TransferFunction_SLOG2 = 6U,
            TransferFunction_BT1886 = 7U,
            TransferFunction_HLG_OETF = 8U,
            TransferFunction_HLG_EOTF = 9U,
            TransferFunction_PQ_EOTF = 10U,
            TransferFunction_PQ_OETF = 11U,
            TransferFunction_DCIP3 = 12U,
            TransferFunction_PAL_OETF = 13U,
            TransferFunction_PAL625_EOTF = 14U,
            TransferFunction_ST240 = 15U,
            TransferFunction_ACESCC = 16U,
            TransferFunction_ACESCCT = 17U,
            TransferFunction_ADOBERGB = 18U,
            TransferFunction_MAX = 0xFFU
        };

        enum AlphaFlags : uint8_t
        {
            AlphaFlags_STRAIGHT = 0U,
            AlphaFlags_PREMULTIPLIED = 1U
        };

        enum SampleDataTypeQualifiers : uint8_t
        {
            SampleDataTypeQualifiers_LINEAR = 1U << 4U,
            SampleDataTypeQualifiers_EXPONENT = 1U << 5U,
            SampleDataTypeQualifiers_SIGNED = 1U << 6U,
            SampleDataTypeQualifiers_FLOAT = 1U << 7U
        };

		struct DataFormatDesc
		{
			uint32_t totalSize{};

			//https://www.khronos.org/registry/DataFormat/specs/1.3/dataformat.1.3.html#DescriptorPrefix
			struct BlockHeader
			{
				uint32_t vendorId : 17;
				uint32_t type : 15;

				uint32_t : 0; // start new word
				uint32_t versionNumber : 16;
				uint32_t blockSize : 16; // = 24 + 16 x #samples

				uint32_t : 0; // start new word
				uint32_t colorModel : 8;
				uint32_t colorPrimaries : 8;
				uint32_t transferFunction : 8;
				uint32_t flags : 8;

				uint32_t : 0; // start new word
				uint32_t texelBlockDimension0 : 8;
				uint32_t texelBlockDimension1 : 8;
				uint32_t texelBlockDimension2 : 8;
				uint32_t texelBlockDimension3 : 8;

				uint64_t : 0; // start new word
				uint64_t bytesPlane0 : 8;
				uint64_t bytesPlane1 : 8;
				uint64_t bytesPlane2 : 8;
				uint64_t bytesPlane3 : 8;
				uint64_t bytesPlane4 : 8;
				uint64_t bytesPlane5 : 8;
				uint64_t bytesPlane6 : 8;
				uint64_t bytesPlane7 : 8;

				BlockHeader();
			};

			struct Sample
            { 
                uint32_t bitOffset : 16;
                uint32_t bitLength : 8;
                uint32_t channelType : 8;

                uint32_t samplePosition0 : 8;
                uint32_t samplePosition1 : 8;
                uint32_t samplePosition2 : 8;
                uint32_t samplePosition3 : 8;

                uint32_t lower;
                uint32_t upper;
            }; // TODO: https://www.khronos.org/registry/DataFormat/specs/1.3/dataformat.1.3.html#_anchor_id_sample_xreflabel_sample_sample_information

			static constexpr uint32_t blockHeaderSize = sizeof(BlockHeader); // 24u
			static constexpr uint32_t sampleSize = sizeof(Sample); // 16u

			struct Block
			{
				uint32_t getSampleCount() const { return (header.blockSize - blockHeaderSize) / sampleSize; }
				void setSampleCount(uint32_t _sampleCount) { header.blockSize = blockHeaderSize + _sampleCount * sampleSize; }

				BlockHeader header{};
				// ...
				// Sample information for the first sample
				// Sample information for the second sample (optional), etc.
				Sample* pSamples = nullptr; // uint32_t[sampleCount]
				Block* pNext = nullptr;
			};

			Block* pBlocks = nullptr; // Linked list

			uint32_t computeSize() const;

            Block* getLastBlock() const;
		};
	} // !slimktx2
} // !ux3d
