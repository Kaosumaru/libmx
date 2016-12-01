#include "Resources.h"
#include <sstream>
#include <fstream>
#include "Graphic/Images/TextureImage.h"
#include "Sound/Sample.h"
#include "Sound/Stream.h"


namespace MX
{

	std::string Resources::openTextFile(const std::string& path)
	{
		std::string input;

		std::ifstream instream(Paths::get().pathToResource(path));

		if (instream.fail())
			return input;

		input = { (std::istreambuf_iterator<char>(instream)),
			std::istreambuf_iterator<char>() };

		return input;
	}

	const std::shared_ptr<Graphic::TextureImage> &Resources::loadImage(const std::string &image)
	{
		auto fit = _imageMap.find(image);
		if (fit != _imageMap.end())
			return fit->second;
		auto path = Paths::get().pathToImage(image);
		auto image_pointer = Graphic::TextureImage::Create(path);
		if (!image_pointer)
			return Resources::loadImageFromSpriteSheet(image);

		fit = _imageMap.insert(std::make_pair(image, image_pointer)).first;
		return fit->second;
	
	}

	const std::shared_ptr<Graphic::TextureImage> &Resources::loadCenteredImage(float cx, float cy, const std::string &image)
	{
		auto fit = _imageMap.find(image);
		if (fit != _imageMap.end())
		{
			fit->second->SetCenter({ cx, cy });
			return fit->second;
		}

		auto path = Paths::get().pathToImage(image);
		auto image_pointer = Graphic::TextureImage::Create(path);
		if (!image_pointer || image_pointer->empty())
		{
			auto& sprite_sheet_image = Resources::loadImageFromSpriteSheet(image);
			if (sprite_sheet_image)
				sprite_sheet_image->SetCenter({ cx, cy });
			return sprite_sheet_image;
		}
		
		image_pointer->SetCenter({ cx, cy });
		fit = _imageMap.insert(std::make_pair(image, image_pointer)).first;
		return fit->second;
	}

	const Sound::Sample::pointer &Resources::loadSound(const std::string &sound)
	{
		auto fit = _sampleMap.find(sound);
		if (fit != _sampleMap.end())
			return fit->second;
		auto sound_pointer = Sound::Sample::Create(Paths::get().pathToSound(sound).c_str());
		fit = _sampleMap.insert(std::make_pair(sound, sound_pointer)).first;
		return fit->second;
	}
	const std::shared_ptr<Sound::Stream> &Resources::loadStream(const std::string &stream)
	{
		auto fit = _streamMap.find(stream);
		if (fit != _streamMap.end())
			return fit->second;
		auto stream_pointer = Sound::Stream::Create(Paths::get().pathToStream(stream).c_str());
		fit = _streamMap.insert(std::make_pair(stream, stream_pointer)).first;
		return fit->second;
	}


	bool Resources::loadSpriteSheet(const std::string& path)
	{
#ifdef WIPSPRITESHEET
		class SpriteSheetSurface : public MX::Graphic::Surface
		{
		public:
			SpriteSheetSurface(const Surface& parent, int x, int y, int w, int h) : MX::Graphic::Surface(parent, x, y, w, h)
			{

			}

			static pointer Create(const Surface::pointer& parent, const MX::Rectangle& rect, const glm::vec2& offset)
			{
				auto ptr = std::make_shared<SpriteSheetSurface>(*parent, (int)rect.x1, (int)rect.y1, (int)rect.width(), (int)rect.height());
				ptr->_centerOffset = offset;
				return ptr;
			}

			void SetCenter(const glm::vec2& center) override { _center = center - _centerOffset; }
		protected:
			glm::vec2 _centerOffset;
		};


		auto abs_path = Paths::get().pathToImage(path.c_str());
		if (!isSpriteSheet(abs_path))
			return false;

		boost::filesystem::path p(abs_path);
		p.replace_extension(".png");





		using namespace JSON;
		auto root = Node::CreateFromFile(abs_path);
		if (!root)
		{
			assert(false);
			return false;
		}
		auto &node = *root;

		Image::Settings::mipmap = node["Resources"]["Settings"]["Mipmap"].getBool();

		auto image_pointer = Graphic::Bitmap::Create(p.generic_string());
		if (!image_pointer)
		{
			Image::Settings::mipmap = false;
			return false;
		}
		

		auto &frames = node["Resources"]["Frames"];
		for (auto &f : frames)
		{
			auto &frame = *f;
			auto name = path + "/" + frame["Name"].getString();
			float cx = frame["OriginalSize"][0].getFloat() / 2.0f, cy = frame["OriginalSize"][1].getFloat() / 2.0f;


			float x = frame["Rect"][0].getFloat(), y = frame["Rect"][1].getFloat();
			float w = frame["Rect"][2].getFloat(), h = frame["Rect"][3].getFloat();

			auto rect = Rectangle::fromWH(x,y,w,h);
			auto region = SpriteSheetSurface::Create(image_pointer, rect, { frame["OriginalRect"][0].getFloat(), frame["OriginalRect"][1].getFloat() });
			region->SetCenter({ cx, cy });

			_imageMap.insert(std::make_pair(name, region));
		}

		Image::Settings::mipmap = false;
		return true;
#endif
		return false;
	}

	bool Resources::isSpriteSheet(const std::string& path)
	{

#ifdef WIPSPRITESHEET
		boost::filesystem::path p(path);
		return boost::filesystem::is_regular_file(p) && p.extension() == ".json";
#endif
		return false;
	}

	const std::shared_ptr<Graphic::TextureImage> &Resources::loadImageFromSpriteSheet(const std::string &path)
	{
#ifdef WIPSPRITESHEET
		static Graphic::Surface::pointer dummy;
		if (path.find(".json") == std::string::npos)
			return dummy;

		boost::filesystem::path p(path);

		while (!p.empty())
		{
			p = p.parent_path();
			loadSpriteSheet(p.generic_string());
		}

		return _imageMap[path];
#endif
		return nullptr;
	}

}