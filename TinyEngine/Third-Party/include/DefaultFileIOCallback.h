// Copyright (c) 2020 UX3D GmbH. All rights reserved.

#pragma once

#include "callbacks.h"

namespace ux3d
{
	namespace slimktx2
	{
		// IOHandle assumbed to be C FILE handle
		class DefaultFileIOCallback
		{
		public:
			Callbacks getCallback() const;

			operator Callbacks() const;

		private:
			static size_t read(void* _pUserData, IOHandle _file, void* _pData, size_t _size);
			static void write(void* _pUserData, IOHandle _file, const void* _pData, size_t _size);
			static size_t tell(void* _pUserData, IOHandle _file);
			static bool seek(void* _pUserData, IOHandle _file, size_t _offset);
		};

	} // !slimktx2
} // !ux3d