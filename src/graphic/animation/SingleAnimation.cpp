#include "SingleAnimation.h"
#include <memory>
#include "utils/Random.h"

using namespace MX;
using namespace MX::Graphic;

SingleAnimation::SingleAnimation()
{
	_total_duration = 0.0f;
}

SingleAnimation::~SingleAnimation()
{
}

SingleAnimation::pointer SingleAnimation::Create()
{
	return std::make_shared<SingleAnimation>();
}

void SingleAnimation::AddFrame(const std::shared_ptr<Image>& image, float duration)
{
	_frames.push_back(Frame(image, duration));
	_total_duration += duration;
	_timeToFrame[miliduration()] = -- _frames.end();
}

void SingleAnimation::Draw(float seconds, const glm::vec2& pos)
{
	frame_for_second(seconds).image->Draw(pos);
}
void SingleAnimation::DrawTinted(float seconds, const glm::vec2& pos, const Color &color)
{
	frame_for_second(seconds).image->DrawTinted(pos, color);
}

void SingleAnimation::DrawRotated(float seconds, const glm::vec2& offset,const glm::vec2& pos, float angle)
{
	frame_for_second(seconds).image->DrawRotated(offset, pos, angle);
}
void SingleAnimation::DrawScaled(float seconds, const glm::vec2& offset,const glm::vec2& pos, const glm::vec2& scale)
{
	frame_for_second(seconds).image->DrawScaled(offset, pos, scale);
}
void SingleAnimation::DrawCentered(float seconds, const glm::vec2& offset, const glm::vec2& pos, const glm::vec2& scale, float angle, const Color &color)
{
	frame_for_second(seconds).image->DrawCentered(offset, pos, scale, angle, color);
}

const SingleAnimation::Frame& SingleAnimation::random_frame()
{
	int random = Random::randomInt(0, (int)frames().size() - 1);
	return frames()[random];
}

const SingleAnimation::Frame& SingleAnimation::frame_for_second(float seconds)
{
	return frame_for_milisecond((unsigned)(seconds * 1000.f));
}
const SingleAnimation::Frame& SingleAnimation::frame_for_milisecond(unsigned mili)
{
{
	return *(_timeToFrame.lower_bound(mili % miliduration())->second);
}
}