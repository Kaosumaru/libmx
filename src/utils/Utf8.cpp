#include "Utf8.h"
#include "utils/utf8/utf8.h"

namespace MX::utf8
{
	uint32_t next_character(const char*& txt)
	{
		return ::utf8::unchecked::next(txt);
	}

	uint32_t next_character(const wchar_t*& txt)
	{
		return *(txt++);
	}
}
