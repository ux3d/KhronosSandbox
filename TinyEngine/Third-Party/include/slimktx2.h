// Copyright (c) 2020 UX3D GmbH. All rights reserved.

#pragma once

#include <new>

#include "dfd.h"
#include "kvd.h"
#include "callbacks.h"
#include "format.h"

namespace ux3d
{
	namespace slimktx2
	{
		// dont use enum class to allow converssion to uint32_t
		enum class CubeMapFace : uint32_t
		{
			Right = 0,
			Left,
			Top,
			Bottom,
			Front,
			Back,

			Xpos = Right,
			Xneg = Left,
			Ypos = Top,
			Yneg = Bottom,
			Zpos = Front,
			Zneg = Back
		};

		struct Header
		{
			static const uint8_t Magic[12];

			uint8_t identifier[12];
			Format vkFormat;
			uint32_t typeSize;
			uint32_t pixelWidth;
			uint32_t pixelHeight;
			uint32_t pixelDepth;
			uint32_t layerCount;
			uint32_t faceCount;
			uint32_t levelCount;
			uint32_t supercompressionScheme;
		};

		struct SectionIndex
		{
			uint32_t dfdByteOffset;
			uint32_t dfdByteLength;
			uint32_t kvdByteOffset;
			uint32_t kvdByteLength;
			uint64_t sgdByteOffset;
			uint64_t sgdByteLength;
		};

		struct LevelIndex
		{
			uint64_t byteOffset;
			uint64_t byteLength;
			uint64_t uncompressedByteLength;
		};

		enum class Result : uint32_t
		{
			Success = 0u,
			NotImplemented,
			IOReadFail,
			IOWriteFail,
			InvalidIdentifier,
			InvalidImageSize, // setImage _byteSize does not match information of specifyFormat
			InvalidLevelIndex,
			InvalidFaceIndex,
			InvalidLayerIndex,
			LevelIndexNotAllocated,
			MipLevelArryNotAllocated,
			DataFormatDescNotAllocated,
			KeyValueDataNotAllocated
		};

		// Serialization API:

		// SlimKTX2 ktx(callbacks);
		// ktx.specifyFormat(Format::R16G16B16A16_SFLOAT, 1024, 1024);
		// ktx.addDFDBlock(...);
		// ktx.allocateMipLevelArray();
		// ktx.setImage(...);
		// ktx.serialize(_file);

		// ktx internally stores mip levels in order from small to total (pixelWidth * pixelHeight) size
		// SlimKTX2 hides this and exposes mip levels like in most GPU APIs where level 0 contains the full image of pixelWidth * pixelHeight, and level 1 half-size image and so on

		class SlimKTX2
		{
		public:
			SlimKTX2() = default;
			SlimKTX2(const Callbacks& _callbacks);
			~SlimKTX2();

			void setCallbacks(const Callbacks& _callbacks);

			Result parse(IOHandle _file);

			Result serialize(IOHandle _file);

			uint32_t getLevelCount() const;
			uint32_t getLayerCount() const;
			uint32_t getFaceCount() const;

			const Header& getHeader() const;
			const DataFormatDesc& getDFD() const;
			const KeyValueData& getKVD() const;

			// fills header and locks format/data-layout for addImage
			Result specifyFormat(Format _vkFormat, uint32_t _width, uint32_t _height, uint32_t _levelCount = 1u, uint32_t _faceCount = 1u, uint32_t _depth = 0u, uint32_t _layerCount = 0u);

			void addDFDBlock(const DataFormatDesc::BlockHeader& _header, const DataFormatDesc::Sample* _pSamples = nullptr, uint32_t _numSamples = 0u);

			void addKeyValue(const void* _key, uint32_t _keyLength, const void* _value, uint32_t _valueLength);

			// allocates all image memory required for setImage
			Result allocateMipLevelArray();

			// compute byte offset withing m_pContainer for the specified level, face and layer indices, requres m_pLevels to be initialized
			uint64_t getFaceImageOffset(uint32_t _level, uint32_t _face, uint32_t _layer) const;

			// copy image data to container (that was allocated by allocateContainer)
			Result setImage(const void* _pData, size_t _byteSize, uint32_t _level, uint32_t _face, uint32_t _layer);

			// returns pointer to face at _level, _layer, _face index, _imageSize is used for validation if _imageSize != 0u
			Result getImage(uint8_t*& _outImageData, uint32_t _level, uint32_t _face, uint32_t _layer, uint64_t _imageSize = 0u) const;

			// free allocated memory, clear members
			void clear();

		private:

			void* allocate(size_t _size);
			void free(void* _pData);

			template<class T>
			T* allocateArray(size_t _count = 1u) { return new(reinterpret_cast<T*>(allocate(sizeof(T) * _count))) T{}; }

			template<class T>
			bool read(IOHandle _file, T* _pData, size_t _count = 1u) { return _pData != nullptr && sizeof(T) * _count == m_callbacks.read(m_callbacks.userData, _file, _pData, sizeof(T) * _count); }

			template<class T>
			void write(IOHandle _file, const T* _pData, size_t _count = 1u) const { m_callbacks.write(m_callbacks.userData, _file, _pData, sizeof(T)* _count); }

			void writePadding(IOHandle _file, size_t _byteSize) const;

			size_t tell(const IOHandle _file);
			bool seek(IOHandle _file, size_t _offset);

			void log(const char* _pFormat, ...);

			uint32_t getKtxLevel(uint32_t _level) const;

			void destroyDFD();
			bool readDFD(IOHandle _file);
			void writeDFD(IOHandle _file) const;

			void destroyKVD();
			bool readKVD(IOHandle _file);
			void writeKVD(IOHandle _file) const;

			void destoryMipLevelArray();

		private:
			Callbacks m_callbacks{};

			Header m_header{};
			SectionIndex m_sections{};

			// in KTX mip level 0 is the smallest, so m_pLevels[0] contains the image with (resolution >> levelCount) pixels
			LevelIndex* m_pLevels = nullptr;

			DataFormatDesc m_dfd{};
			KeyValueData m_kvd{};

			// mipLevel array
			uint8_t** m_pMipLevelArray = nullptr;
		};
	}// !slimktx2
} // ux3d
