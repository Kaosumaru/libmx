#pragma once
#include <string>
#include <map>
#include <memory>
#include "Paths.h"

namespace MX
{
	namespace Graphic
	{
		class TextureImage;
		class BitmapFont;
		class Blender;
	}

	namespace Sound
	{
		class Sample;
		class Stream;
	}

class Resources : public Singleton<Resources>
{
public:
	const std::shared_ptr<Graphic::TextureImage>& loadImage(const std::string &image);
	const std::shared_ptr<Graphic::TextureImage>& loadCenteredImage(float cx, float cy, const std::string &image);
	const std::shared_ptr<Sound::Sample>& loadSound(const std::string &sound);
	const std::shared_ptr<Sound::Stream>& loadStream(const std::string &stream);
	const std::shared_ptr<Graphic::BitmapFont>& loadBitmapFont(const std::string &font);
	bool loadSpriteSheet(const std::string& path);
	bool isSpriteSheet(const std::string& path);

	std::string openTextFile(const std::string& path);

	const std::shared_ptr<Graphic::TextureImage>& whiteSurface();

    void Clear();
protected:
	const std::shared_ptr<Graphic::TextureImage> &loadImageFromSpriteSheet(const std::string &path);

	std::map<std::string, std::shared_ptr<Graphic::TextureImage>> _imageMap;
	std::map<std::string, std::shared_ptr<Sound::Sample>> _sampleMap;
	std::map<std::string, std::shared_ptr<Sound::Stream>> _streamMap;
	std::map<std::string, std::shared_ptr<Graphic::BitmapFont>> _bitmapFontMap;
};

}
