#pragma once
#include "graphic/images/Image.h"
#include "utils/Utils.h"
#include <list>
#include <map>
#include <vector>

namespace MX
{
namespace Graphic
{

    class SingleAnimation : public shared_ptr_init<SingleAnimation>, virtual public disable_copy_constructors
    {
    protected:
        struct Frame
        {
            Frame()
                : image(nullptr)
                , duration(0.0f)
            {
            }
            Frame(const std::shared_ptr<Image>& _image, float _duration)
                : image(_image)
                , duration(_duration)
            {
            }

            const std::shared_ptr<Image> image;
            const float duration;
        };

    public:
        using Frames = std::vector<Frame>;
        SingleAnimation();
        virtual ~SingleAnimation();
        static pointer Create();

        void AddFrame(const std::shared_ptr<Image>& image, float duration);

        virtual void Draw(float seconds, const glm::vec2& pos);
        virtual void DrawTinted(float seconds, const glm::vec2& pos, const Color& color);
        virtual void DrawRotated(float seconds, const glm::vec2& offset, const glm::vec2& pos, float angle);
        virtual void DrawScaled(float seconds, const glm::vec2& offset, const glm::vec2& pos, const glm::vec2& scale);
        virtual void DrawCentered(float seconds, const glm::vec2& offset, const glm::vec2& pos, const glm::vec2& scale = { 1.0f, 1.0f }, float angle = 0.0f, const Color& color = Color());

        const Frames& frames() { return _frames; }
        const Frame& frame_for_second(float seconds);
        const Frame& frame_for_milisecond(unsigned mili);
        float duration() { return _total_duration; }
        unsigned miliduration() { return (unsigned)(_total_duration * 1000.); }

        const Frame& random_frame();

        void SetDistanceHelper(float value) { _distanceHelper = value; }
        float distanceHelper() { return _distanceHelper; }

    protected:
        float _distanceHelper = 0.0f;
        Frames _frames;
        std::map<unsigned, Frames::iterator> _timeToFrame;
        float _total_duration;
    };

}
}
