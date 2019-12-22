#include "Paths.h"

namespace MX
{

Paths::Paths()
{
}

std::string Paths::realPathToResourcePath(const std::string& path)
{
    auto it = path.find(_resourcePath);
    if (it == std::string::npos)
        return path;
    auto p = path;
    p.erase(it, it + _resourcePath.size());
    return p;
}

std::string Paths::pathToResource(const std::string& resource)
{
    if (resource.size() > 2 && resource[1] == ':')
        return resource;
    return _resourcePath + resource;
}

std::string Paths::pathToImage(const std::string& image)
{
    return _imagePath + image;
}
std::string Paths::pathToSound(const std::string& sound)
{
    return _soundPath + sound;
}
std::string Paths::pathToStream(const std::string& stream)
{
    return _streamPath + stream;
}
std::string Paths::pathToFont(const std::string& font)
{
    return _fontPath + font;
}

void Paths::SetResourcePath(const std::string& path)
{
    _resourcePath = path;
    _fontPath = _resourcePath;
}

void Paths::SetImagePath(const std::string& path)
{
    _imagePath = path;
}
void Paths::SetSoundPath(const std::string& path)
{
    _soundPath = path;
}
void Paths::SetStreamPath(const std::string& path)
{
    _streamPath = path;
}
void Paths::SetFontPath(const std::string& path)
{
    _fontPath = path;
}

}