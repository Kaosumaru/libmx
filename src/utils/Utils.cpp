#include "Utils.h"
#include <codecvt>
#include <locale>

namespace MX
{
std::wstring stringToWide(const std::string& str)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    return convert.from_bytes(str);
}

std::string wideToUTF(const std::wstring& str)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    return convert.to_bytes(str);
}
}
