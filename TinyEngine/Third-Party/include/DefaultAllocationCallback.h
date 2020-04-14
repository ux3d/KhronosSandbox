// Copyright (c) 2020 UX3D GmbH. All rights reserved.

#pragma once

#include "callbacks.h"

namespace ux3d
{
	namespace slimktx2
	{
		class DefaultAllocationCallback
		{
		public:
			Callbacks getCallback() const;

			operator Callbacks() const;

		private:
			static void* allocate(void* _pUserData, size_t _size);
			static void deallocate(void* _pUserData, void* _pData);
		};
	} // !slimktx2
} // !ux3d