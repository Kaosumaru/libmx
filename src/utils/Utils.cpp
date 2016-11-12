#include "Utils.h"
#include <codecvt>
#include <locale>

std::wstring stringToWide(const std::string &str)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    return convert.from_bytes(str);
}