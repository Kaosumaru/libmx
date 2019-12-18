#include "ScriptAnimationClass.h"
#include "game/resources/Resources.h"
#include "graphic/Blender.h"
#include "graphic/animation/Animation.h"
#include "script/PropertyLoaders.h"
#include "script/Script.h"
#include "utils/Log.h"

using namespace MX;

ScriptAnimationClass::ScriptAnimationClass()
{
}

const std::shared_ptr<Graphic::SingleAnimation>& ScriptAnimationClass::animation() const
{
    return _animation;
}

std::shared_ptr<Graphic::Image> ScriptAnimationClass::image() const
{
    return MX::Graphic::Animation::Create(_animation);
}

ScriptAnimationFromFilesClass::ScriptAnimationFromFilesClass()
{
}

bool ScriptAnimationFromFilesClass::onParse()
{
    int offset = 0;
    float distance_helper = 0.0f;
    load_property(distance_helper, "Distance");

    load_property(_frameDuration, "Frame_duration");
    load_property(_framesCount, "Frames_count");
    load_property(offset, "Frames_offset");
    load_property(_filenameTemplate, "Filename_template");
    load_property(_pivot, "Frame_pivot");

    load_property(MX::Graphic::Image::Settings::mipmap, "Mipmap");

    bool found_blender = false;
    MX::Graphic::Blender blender(0, 0, 0);

    if (load_property(blender, "Blend"))
    {
        spdlog::error("ScriptAnimationFromFilesClass::onParse canoot load blender!");
    }

    auto load_without_blender = [&](const char* image) { return Resources::get().loadCenteredImage(_pivot.x, _pivot.y, image); };

    std::function<std::shared_ptr<Graphic::Image>(const char* image)> loadImage;

    loadImage = load_without_blender;

    _animation = Graphic::SingleAnimation::Create();
    _animation->SetDistanceHelper(distance_helper);
    char tmp[256];
    for (unsigned i = 0; i < _framesCount; i++)
    {
        sprintf(tmp, _filenameTemplate.c_str(), i + offset);
        auto image = loadImage(tmp);
        assert(image);
        _animation->AddFrame(image, _frameDuration);
    }

    std::vector<std::string> frames;
    load_property(frames, "Frames");
    for (auto& frame : frames)
    {
        _animation->AddFrame(loadImage(frame.c_str()), _frameDuration);
    }

    MX::Graphic::Image::Settings::mipmap = false;
    return true;
}

ScriptAnimationFromFileClass::ScriptAnimationFromFileClass()
{
}

bool ScriptAnimationFromFileClass::onParse()
{
    float distance_helper = 0.0f;
    load_property(distance_helper, "Distance");

    std::string path;
    load_property(path, "Filename_template");
    auto image = Resources::get().loadImage(path.c_str());

    //sodo assert
    if (!image)
        return true;

    float frameDuration = 0.0f;

    load_property(frameDuration, "Frame_duration");
    load_property(_pivot, "Frame_pivot");

    int rows = 1, columns = 1;
    load_property(rows, "Rows");
    load_property(columns, "Columns");

    glm::vec2 offset, frame;

    load_property(offset, "Offset");
    load_property(frame, "Frame");

    _animation = Graphic::SingleAnimation::Create();
    _animation->SetDistanceHelper(distance_helper);

    for (int y = 0; y < rows; y++)
        for (int x = 0; x < columns; x++)
        {
            auto f = MX::Graphic::TextureImage::Create(image, Rectangle::fromWH(x * frame.x + offset.x, y * frame.y + offset.y, frame.x, frame.y));
            f->SetCenter(_pivot);
            _animation->AddFrame(f, frameDuration);
        }

    return true;
}

ScriptRandomAnimation::ScriptRandomAnimation()
{
}

bool ScriptRandomAnimation::onParse()
{
    load_property(_animations, "Animations");
    return true;
}

const std::shared_ptr<Graphic::SingleAnimation>& ScriptRandomAnimation::animation() const
{
    static std::shared_ptr<Graphic::SingleAnimation> dummy;
    if (_animations.empty())
        return dummy;
    return _animations[rand() % _animations.size()];
}
