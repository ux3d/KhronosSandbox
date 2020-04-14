// Copyright (c) 2020 UX3D GmbH. All rights reserved.

#pragma once

#include <cstdint>

namespace ux3d
{
	namespace slimktx2
	{
		struct KeyValueData
		{
			static constexpr auto KTXwriterKey = "KTXwriter";
			static constexpr auto KTXwriterKeyLength = sizeof("KTXwriter");

			static constexpr auto KTXwriterValue = "UX3D SlimKTX2 v1.0";
			static constexpr auto KTXwriterValueLength = sizeof("UX3D SlimKTX2 v1.0");

			struct Entry
			{
				uint32_t keyAndValueByteLength = 0u;
				
				uint8_t* pKeyValue = nullptr;
				
				uint32_t getKeyLength() const;
				uint32_t getValueLength() const;	

				Entry* pNext = nullptr;
			};

			Entry* pKeyValues = nullptr; // linked list
			uint32_t computeSize() const; // includes per-entry padding
			Entry* getLastEntry() const;
		};
	} // !slimktx2
} // ux3d
