#include "SpriteScene.h"
#include "Application/Window.h"

using namespace std;
using namespace MX;

SpriteScene::SpriteScene()
{

}

void SpriteScene::Run()
{
}
void SpriteScene::AddActor(const SpriteActorPtr &actor)
{
	assert(false);
}

void SpriteScene::AddActorAtFront(const SpriteActorPtr &actor)
{
	assert(false);
}

void SpriteScene::Draw(float x, float y)
{
}

void SpriteScene::DrawCustom(float x, float y)
{
}


void SpriteScene::SetVisible(bool visible)
{
	if (visible == _visible)
		return;
	_visible = !_visible;

	if (visible)
		ChangeVisibility(-1);
	else
		ChangeVisibility(1);
}


void SpriteScene::ChangeVisibility(int i)
{
	if (i == 0)
		return;
	bool vis = visible();
	_visibility += i;



	if (visible() == vis)
		return;

	for_each<SpriteScene>([=](const std::shared_ptr<SpriteScene>& scene) { scene->ChangeVisibility(visible() ? -1 : 1); });

	if (visible())
		onBecameVisible();
	else
		onBecameInvisible();
}


void SpriteScene::LinkedActor(const Actor::pointer &actor)
{
	auto scene = std::dynamic_pointer_cast<SpriteScene>(actor);
	if (scene && visible())
	{
		scene->ChangeVisibility(-1);
	}
}
void SpriteScene::UnlinkedActor(Actor &actor)
{
	auto scene = dynamic_cast<SpriteScene*>(&actor);
	if (scene && visible())
	{
		scene->ChangeVisibility(1); //TODO this seems unlogical
	}
}

void SpriteScene::onBecameVisible()
{
}

void SpriteScene::onBecameInvisible()
{
}

void SpriteScene::translate_child_position(glm::vec2& position)
{
	position += geometry.position;
	if (_scene)
		sprite_scene().translate_child_position(position);
}

bool SpriteScene::visible()
{
	return _visibility < 0;
}

BaseGraphicScene::BaseGraphicScene()
{

}





DisplayScene::DisplayScene(const glm::vec2& size) : _size(size)
{
	
}

void DisplayScene::Draw(float x, float y)
{

	BaseGraphicScene::Draw(x,y);
}

float DisplayScene::Width()
{
	return _size.x;
}
float DisplayScene::Height()
{
	return _size.y;
}


FullscreenDisplayScene::FullscreenDisplayScene() : DisplaySceneTimer(MX::Window::current().size())
{

}

void FullscreenDisplayScene::Run()
{
	_size = MX::Window::current().size();
	DisplaySceneTimer::Run();
}


DisplaySceneTimer::DisplaySceneTimer(const glm::vec2& size) : DisplayScene(size)
{
	_paused = false;
}
void DisplaySceneTimer::Run()
{
	Run(1.0f);
}

void DisplaySceneTimer::Run(float timeMultiplier)
{
	if (_paused && !visible())
		return;

	auto guard = Context<Time::Timer>::Lock(_timer);
	auto guard2 = Context<FunctorsQueue>::Lock(_queue);
	_queue.Run();
	DisplayScene::Run();
	_timer.Step(timeMultiplier);
}

Time::SimpleTimer &DisplaySceneTimer::timer()
{
    return _timer;
}

FunctorsQueue &DisplaySceneTimer::queue()
{
    return _queue;
}

void DisplaySceneTimer::Draw(float x, float y)
{
	auto guard = Context<Time::Timer>::Lock(_timer);
	DisplayScene::Draw(x, y);
}

void DisplaySceneTimer::Pause(bool paused)
{
	_paused = paused;
	_timer.Pause(paused);
}

bool DisplaySceneTimer::paused()
{
    return _paused;
}


#ifdef WIP
BitmapDisplaySceneTimer::BitmapDisplaySceneTimer(const glm::vec2& size) : DisplaySceneTimer(size)
{
	_bitmap = Graphic::Surface::Create(size.x, size.y);
}
void BitmapDisplaySceneTimer::Draw(float x, float y)
{
	if (!visible())
		return;
	{
		Graphic::TargetContext context(*_bitmap);
		DisplayScene::Draw(0.0f,0.0f);
	}
	_bitmap->DrawCentered(0.0f, 0.0f, x, y, geometry.scale.x, geometry.scale.y, geometry.angle, geometry.color);
}
unsigned BitmapDisplaySceneTimer::Width()
{
	return _bitmap->Width();
}
unsigned BitmapDisplaySceneTimer::Height()
{
	return _bitmap->Height();
}


BitmapScene::BitmapScene(const glm::vec2& size, const Color &backgroundColor, bool alpha) : _backgroundColor(backgroundColor)
{
	_bitmap = Graphic::Surface::Create(size.x, size.y, alpha);
}

void BitmapScene::Draw(float x, float y)
{
	if (!visible())
		return;
	{
		Graphic::TargetContext context(*_bitmap);
		_bitmap->Clear(_backgroundColor);
		BaseGraphicScene::Draw(0.0f,0.0f);
	}
	_bitmap->DrawCentered(0.0f, 0.0f, x, y, geometry.scale.x, geometry.scale.y, geometry.angle, geometry.color);
}

unsigned BitmapScene::Width()
{
	return _bitmap->Width();
}
unsigned BitmapScene::Height()
{
	return _bitmap->Height();
}
#endif