#pragma once
#include <cstdint>

namespace MX
{
	namespace utf8
	{
		uint32_t next_character(const char*& txt);
		uint32_t next_character(const wchar_t*& txt);
	}
}

