#include "Archive.h"
#include "FileArchive.h"
#include "EmscriptenArchive.h"

using namespace MX;
using namespace Serialization;

std::unique_ptr<Archive> Archive::CreateDefault(const std::string& path)
{
#ifdef __EMSCRIPTEN__
	return std::make_unique<EmscriptenArchive>(path);
#else
	return std::make_unique<FileArchive>(path);
#endif
}