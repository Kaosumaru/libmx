#include "Animation.h"
#include "utils/Time.h"
#include <memory>

using namespace std;
using namespace MX;
using namespace MX::Graphic;

Animation::Animation()
{

    _parent_animation = nullptr;
}

MX::Graphic::Animation::Animation(const Animation& animation)
{
    _parent_animation = animation._parent_animation;
    SetCurrentFrame(animation.current_frame_index());
}

Animation::Animation(const SingleAnimation::pointer& parent_animation)
{

    _parent_animation = parent_animation;
    _currentFrame = parent_animation->frames().begin();
}
Animation::~Animation()
{
}

Animation::pointer Animation::Create(const SingleAnimation::pointer& parent_animation)
{
    if (parent_animation->frames().empty())
        return nullptr;
    return std::make_shared<Animation>(parent_animation);
}

void Animation::Start()
{
    _running = true;
}
void Animation::Stop()
{
    _running = false;
}

void Animation::Reset()
{
    _currentTime = 0.0f;
    _currentFrame = _parent_animation->frames().begin();
    Start();
}

void Animation::Reset(const SingleAnimation::pointer& parent_animation)
{
    if (parent_animation)
    {
        _parent_animation = parent_animation;
        Reset();
    }
    else
        Clear();
}

void Animation::Clear()
{
    _parent_animation = nullptr;
    _running = false;
    _currentTime = 0.0f;
}

void Animation::AdvanceTime(float seconds)
{
    if (!_running)
        return;
    if (seconds < 0)
    {
        return;
    }
    _currentTime += seconds;
    ChangeFrame();
}

void Animation::AdvanceTimeScaleByTraveledPixels(float seconds, float traveled_pixels, float animation_scale)
{
    if (seconds <= 0.0f || !_parent_animation)
        return;
    if (_parent_animation->distanceHelper() == 0.0f)
        return AdvanceTime(seconds);

    float pps = (1.0f / seconds) * traveled_pixels;
    auto ratio = pps / _parent_animation->distanceHelper();
    AdvanceTime(seconds * ratio / animation_scale);
}

bool Animation::running()
{
    return _running;
}

void Animation::ChangeFrame()
{
    if (!_parent_animation)
        return;
    auto& frames = _parent_animation->frames();

    while (_currentTime > _currentFrame->duration)
    {
        _currentTime -= _currentFrame->duration;
        _currentFrame++;
        if (_currentFrame == frames.end())
            _currentFrame = frames.begin();
    }
}

//draw
void Animation::Draw(const glm::vec2& pos)
{
    if (!_parent_animation)
        return;
    _currentFrame->image->Draw(pos);
}
void Animation::DrawTinted(const glm::vec2& pos, const Color& color)
{
    if (!_parent_animation)
        return;
    _currentFrame->image->DrawTinted(pos, color);
}

void Animation::DrawRotated(const glm::vec2& offset, const glm::vec2& pos, float angle)
{
    if (!_parent_animation)
        return;
    _currentFrame->image->DrawRotated(offset, pos, angle);
}

void Animation::DrawScaled(const glm::vec2& offset, const glm::vec2& pos, const glm::vec2& scale)
{
    if (!_parent_animation)
        return;
    _currentFrame->image->DrawScaled(offset, pos, scale);
}

void Animation::DrawCentered(const glm::vec2& offset, const glm::vec2& pos, const glm::vec2& scale, float angle, const Color& color)
{
    if (!_parent_animation)
        return;
    _currentFrame->image->DrawCentered(offset, pos, scale, angle, color);
}

unsigned Animation::current_frame_index() const
{
    if (!_parent_animation)
        return 0;
    auto& frames = _parent_animation->frames();
    return std::distance(frames.begin(), _currentFrame);
}

const SingleAnimation::pointer& Animation::parent_animation() const
{
    return _parent_animation;
}

const std::shared_ptr<Image>& Animation::current_frame_image() const
{
    static std::shared_ptr<Image> image;
    if (!_parent_animation)
        return image;
    if (_currentFrame == _parent_animation->frames().end())
    {
        auto it = _parent_animation->frames().begin();
        if (_parent_animation->frames().end() == it)
            return image;
        return it->image;
    }

    return _currentFrame->image;
}

void Animation::SetCurrentFrame(unsigned index)
{
    _currentTime = 0;
    _currentFrame = _parent_animation->frames().begin() + index;
}

Animation::pointer Animation::clone() const
{
    return Create(_parent_animation);
}

AnimationSingleRun::AnimationSingleRun(const SingleAnimation::pointer& parent_animation)
    : Animation(parent_animation)
{
    _running = true;
}
void AnimationSingleRun::ChangeFrame()
{
    if (!_parent_animation)
        return;
    auto& frames = _parent_animation->frames();

    while (_currentTime > _currentFrame->duration)
    {
        _currentTime -= _currentFrame->duration;
        _currentFrame++;
        if (_currentFrame == frames.end())
        {
            _currentFrame = --frames.end();
            _running = false;
            return;
        }
    }
}

Animation::pointer AnimationSingleRun::clone() const
{
    return std::make_shared<AnimationSingleRun>(_parent_animation);
}
