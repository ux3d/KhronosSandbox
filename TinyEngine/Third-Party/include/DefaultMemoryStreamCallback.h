// Copyright (c) 2020 UX3D GmbH. All rights reserved.

#pragma once

#include "callbacks.h"
#include <cstdint>

namespace ux3d
{
	namespace slimktx2
	{
		// 'IOHanlde'
		class DefaultMemoryStream
		{
		public:
			// read constructor
			DefaultMemoryStream(const uint8_t* _pData, const size_t _size) :
				m_pDataConst(_pData), m_size(_size) {}

			// read write
			DefaultMemoryStream(uint8_t* _pData, const size_t _size) :
				m_pDataConst(_pData), m_pData(_pData), m_size(_size) {}

			const uint8_t* getDataConst() const { return m_pDataConst; };
			uint8_t* getData() const { return m_pData; };

			size_t getSize() const { return m_size; };

			size_t getOffset() const { return m_offset; };
			void setOffset(size_t _offset) { m_offset = _offset; };

		private:
			const uint8_t* m_pDataConst = nullptr;
			uint8_t* m_pData = nullptr;
			const size_t m_size = 0u;
			size_t m_offset = 0u;
		};

		// assumes IOHandle is of type DefaultMemoryStream
		class DefaultMemoryStreamCallback
		{
		public:
			Callbacks getCallback() const;

			operator Callbacks() const;

		private:
			static size_t read(void* _pUserData, IOHandle _iohandle, void* _pData, size_t _size);
			static void write(void* _pUserData, IOHandle _iohandle, const void* _pData, size_t _size);
			static size_t tell(void* _pUserData, IOHandle _iohandle);
			static bool seek(void* _pUserData, IOHandle _iohandle, size_t _offset);
		};
	} // !slimktx2
} // !ux3d
