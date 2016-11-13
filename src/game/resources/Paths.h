#pragma once
#include <string>
#include "utils/Singleton.h"

namespace MX
{


class Paths : public Singleton<Paths>
{
public:
	Paths();

	std::string pathToResource(const std::string &resource);
	std::string pathToImage(const std::string &image);
	std::string pathToSound(const std::string &sound);
	std::string pathToStream(const std::string &stream);
	std::string pathToFont(const std::string &font);

	void SetResourcePath(const std::string& path);
	void SetImagePath(const std::string& path);
	void SetSoundPath(const std::string& path);
	void SetStreamPath(const std::string& path);
	void SetFontPath(const std::string& path);

protected:
	std::string _resourcePath;
	std::string _imagePath;
	std::string _soundPath;
	std::string _streamPath;
	std::string _fontPath;
};

}