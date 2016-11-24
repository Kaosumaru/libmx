#include "SceneTransition.h"
#include <iostream>

using namespace std;

namespace MX
{
	using namespace MX::Graphic;
	BaseBitmapTransition::BaseBitmapTransition(float time)
	{
		_time = time;
	}

	void BaseBitmapTransition::SetScenes(const std::shared_ptr<SpriteScene> &old_scene, const std::shared_ptr<SpriteScene> &new_scene)
	{
#if WIPBLENDER
		auto guard = Graphic::Blender::defaultNormal().Use();
#endif
		if (old_scene)
		{
			_scene1 = TextureImage::Create(old_scene->Width(), old_scene->Height());
			{
				Graphic::TargetContext context(*_scene1);
				_scene1->Clear(MX::Color(0.0f, 0.0f, 0.0f));
				old_scene->Draw(0.0f, 0.0f);
			}
		}

		if (new_scene)
		{
			_scene2 = TextureImage::Create(new_scene->Width(), new_scene->Height());
			{
				Graphic::TargetContext context(*_scene2);
				_scene2->Clear(MX::Color(0.0f, 0.0f, 0.0f));
				new_scene->Draw(0.0f, 0.0f);
			}
		}
	}

	void BaseBitmapTransition::Run()
	{
		if (!_stopWatch.isSet())
			_stopWatch.Start(_time);
		else if (_stopWatch.Tick())
			Unlink();
	}

	void BaseBitmapTransition::Draw(float x, float y)
	{

	}

	AlphaBitmapTransition::AlphaBitmapTransition(float time) : BaseBitmapTransition(time)
	{
	}


	void AlphaBitmapTransition::Draw(float x, float y)
	{
		if (_scene1)
			_scene1->DrawTinted(0.0f, 0.0f, Color(1.0f, 1.0f, 1.0f, 1.0f-_stopWatch.percent()));
		if (_scene2)
			_scene2->DrawTinted(0.0f, 0.0f, Color(1.0f, 1.0f, 1.0f, _stopWatch.percent()));
	}




	MoveBitmapTransition::MoveBitmapTransition(bool left, float time) : BaseBitmapTransition(time)
	{
		_left = left;
	}
	
	void MoveBitmapTransition::Draw(float x, float y)
	{
		auto transition = _stopWatch.exponential();
		auto inverse_transition = 1.0f - transition;

		if (_left)
		{
			if (_scene1)
				_scene1->Image::Draw(-transition*(float)_scene1->Width(), 0.0f);
			if (_scene2)
				_scene2->Image::Draw(inverse_transition*(float)_scene1->Width(), 0.0f);
		}
		else
		{
			if (_scene1)
				_scene1->Image::Draw(transition*(float)_scene2->Width(), 0.0f);
			if (_scene2)
				_scene2->Image::Draw(-inverse_transition*(float)_scene2->Width(), 0.0f);
		}
	}

}