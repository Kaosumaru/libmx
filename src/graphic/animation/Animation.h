#pragma once
#include "graphic/images/Image.h"
#include "graphic/animation/SingleAnimation.h"
#include "utils/Utils.h"
#include <list>
#include <map>


namespace MX{
namespace Graphic
{


class Animation : public Image, public shared_ptr_init<Animation>
{
public:
    Animation();
	Animation(const Animation& animation);
	Animation(const SingleAnimation::pointer& parent_animation);
	virtual ~Animation();
	static pointer Create(const SingleAnimation::pointer& parent_animation);

    virtual void Start();
    virtual void Stop();
	virtual bool running();
    virtual void Reset();
	virtual void Reset(const SingleAnimation::pointer& parent_animation);
	void AdvanceTime(float seconds) override;
	void AdvanceTimeScaleByTraveledPixels(float seconds, float traveled_pixels, float animation_scale);

	bool empty() const { return _parent_animation == nullptr; }
	void Clear();

	//draw
	void Draw(const glm::vec2& pos) override;
	void DrawTinted(const glm::vec2& pos, const Color &color) override;
	void DrawRotated(const glm::vec2& offset, const glm::vec2& pos, float angle) override;
	void DrawScaled(const glm::vec2& offset,const glm::vec2& pos, const glm::vec2& scale) override;
	void DrawCentered(const glm::vec2& offset, const glm::vec2& pos, const glm::vec2& scale = { 1.0f, 1.0f }, float angle = 0.0f, const Color &color = Color()) override;

	unsigned Height() override { return -1; }
	unsigned Width() override { return -1; }

	virtual Animation::pointer clone() const;

	const SingleAnimation::pointer& parent_animation() const;

	const std::shared_ptr<Image>& current_frame_image() const;
	unsigned current_frame_index() const;
	void SetCurrentFrame(unsigned index);
protected:
	virtual void ChangeFrame();

    bool _running = true;
	float _currentTime = 0.0f;

	SingleAnimation::Frames::const_iterator _currentFrame;
	SingleAnimation::pointer _parent_animation;
};
    
class AnimationSingleRun : public Animation
{
public:
    AnimationSingleRun(const SingleAnimation::pointer& parent_animation);
    void ChangeFrame() override;
	Animation::pointer clone() const override;
};

}
}
