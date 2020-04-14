// Copyright (c) 2020 UX3D GmbH. All rights reserved.

#pragma once

#include <cstdarg>
#include <cstddef>

namespace ux3d
{
	namespace slimktx2
	{
		using AllocationFunc = void* (*)(void* _pUserData, size_t _size);
		using DeallocationFunc = void(*)(void* _pUserData, void* _pData);

		// IO - stream or file
		using IOHandle = void*;

		using ReadFunc = size_t(*)(void* _pUserData, IOHandle _file, void* _pData, size_t _size);
		using WriteFunc = void(*)(void* _pUserData, IOHandle _file, const void* _pData, size_t _size);
		using TellFunc = size_t(*)(void* _pUserData, const IOHandle _file);
		using SeekFunc = bool(*)(void* _pUserData, IOHandle _file, size_t _offset);

		using LogFunc = void(*)(void* _pUserData, const char* _pFormat, va_list args);

		struct Callbacks
		{
			void* userData = nullptr; // holds allocator, user implementations

			AllocationFunc allocate = nullptr;
			DeallocationFunc deallocate = nullptr;

			ReadFunc read = nullptr;
			WriteFunc write = nullptr;

			TellFunc tell = nullptr;
			SeekFunc seek = nullptr;

			// optional
			LogFunc log = nullptr;
		};

		inline Callbacks operator|(const Callbacks& _lhs, const Callbacks& _rhs)
		{
			Callbacks callback = _lhs;

			if (callback.userData == nullptr) { callback.userData = _rhs.userData; }
			if (callback.allocate == nullptr) { callback.allocate = _rhs.allocate; }
			if (callback.deallocate == nullptr) { callback.deallocate = _rhs.deallocate; }
			if (callback.read == nullptr) { callback.read = _rhs.read; }
			if (callback.write == nullptr) { callback.write = _rhs.write; }
			if (callback.tell == nullptr) { callback.tell = _rhs.tell; }
			if (callback.seek == nullptr) { callback.seek = _rhs.seek; }
			if (callback.log == nullptr) { callback.log = _rhs.log; }
			
			return callback;
		}
	}// !slimktx2
} // ux3d
