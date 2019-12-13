#include "Resources.h"
#include "deps/json/json.h"
#include "graphic/fonts/BitmapFont.h"
#include "graphic/images/TextureImage.h"
#include "sound/Sample.h"
#include "sound/Stream.h"
#include "deps/json/json_cpp.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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

void Resources::Clear()
{
    _imageMap.clear();
    _sampleMap.clear();
    _streamMap.clear();
}

const std::shared_ptr<Graphic::TextureImage>& Resources::loadImage(const std::string& image)
{
    auto fit = _imageMap.find(image);
    if (fit != _imageMap.end())
        return fit->second;
    auto path = Paths::get().pathToImage(image);
    auto image_pointer = Graphic::TextureImage::Create(path);
    if (!image_pointer)
    {
        auto& res = Resources::loadImageFromSpriteSheet(image);
        if (!res)
            std::cout << "Couldn't load " << image << std::endl;
        return res;
    }

    fit = _imageMap.insert(std::make_pair(image, image_pointer)).first;
    return fit->second;
}

const std::shared_ptr<Graphic::TextureImage>& Resources::loadCenteredImage(float cx, float cy, const std::string& image)
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

const Sound::Sample::pointer& Resources::loadSound(const std::string& sound)
{
    auto fit = _sampleMap.find(sound);
    if (fit != _sampleMap.end())
        return fit->second;
    auto sound_pointer = Sound::Sample::Create(Paths::get().pathToSound(sound).c_str());
    fit = _sampleMap.insert(std::make_pair(sound, sound_pointer)).first;
    return fit->second;
}

const std::shared_ptr<Sound::Stream>& Resources::loadStream(const std::string& stream)
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
    class SpriteSheetTexture : public MX::Graphic::TextureImage
    {
    public:
        SpriteSheetTexture(const TextureImage& parent, int x, int y, int w, int h)
            : MX::Graphic::TextureImage(parent, x, y, w, h)
        {
        }

        static pointer Create(const MX::Graphic::TextureImage::pointer& parent, const MX::Rectangle& rect, const glm::vec2& offset)
        {
            auto ptr = std::make_shared<SpriteSheetTexture>(*parent, (int)rect.x1, (int)rect.y1, (int)rect.width(), (int)rect.height());
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

    std::filesystem::path spriteSheetPath(abs_path);
    spriteSheetPath.replace_extension(".png");

    using namespace JSON;
    auto root = Node::CreateFromFile(abs_path);
    if (!root)
    {
        assert(false);
        return false;
    }
    auto& node = *root;

    MX::Graphic::Image::Settings::mipmap = node["Resources"]["Settings"]["Mipmap"].getBool();

    auto image_pointer = MX::Graphic::TextureImage::Create(spriteSheetPath.generic_string());
    if (!image_pointer)
    {
        MX::Graphic::Image::Settings::mipmap = false;
        return false;
    }

    auto& frames = node["Resources"]["Frames"];
    for (auto& f : frames)
    {
        auto& frame = *f;
        std::string name = path + "/" + frame["Name"].getString();
        float cx = frame["OriginalSize"][0].getFloat() / 2.0f, cy = frame["OriginalSize"][1].getFloat() / 2.0f;

        float x = frame["Rect"][0].getFloat(), y = frame["Rect"][1].getFloat();
        float w = frame["Rect"][2].getFloat(), h = frame["Rect"][3].getFloat();

        auto rect = Rectangle::fromWH(x, y, w, h);
        auto region = SpriteSheetTexture::Create(image_pointer, rect, { frame["OriginalRect"][0].getFloat(), frame["OriginalRect"][1].getFloat() });
        region->SetCenter({ cx, cy });

        _imageMap.insert(std::make_pair(name, region));
    }

    MX::Graphic::Image::Settings::mipmap = false;
    return true;
}

bool Resources::isSpriteSheet(const std::string& path)
{
    std::filesystem::path p(path);
    return std::filesystem::is_regular_file(p) && p.extension() == ".json";
}

const std::shared_ptr<Graphic::TextureImage>& Resources::loadImageFromSpriteSheet(const std::string& path)
{
    static Graphic::TextureImage::pointer dummy;
    if (path.find(".json") == std::string::npos)
        return dummy;

    std::filesystem::path p(path);

    while (!p.empty())
    {
        p = p.parent_path();
        loadSpriteSheet(p.generic_string());
    }

    return _imageMap[path];
}

const std::shared_ptr<Graphic::BitmapFont>& Resources::loadBitmapFont(const std::string& font)
{
    auto fit = _bitmapFontMap.find(font);
    if (fit != _bitmapFontMap.end())
        return fit->second;
    auto font_pointer = Graphic::BitmapFont::Create(font);
    fit = _bitmapFontMap.insert(std::make_pair(font, font_pointer)).first;
    return fit->second;
}

const std::shared_ptr<Graphic::TextureImage>& Resources::whiteSurface()
{
    return loadImage("misc/White128.png");
}
}