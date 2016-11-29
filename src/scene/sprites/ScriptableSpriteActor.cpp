#include "ScriptableSpriteActor.h"
#include <iostream>
#include "Utils/Time.h"
#include "scene/Generators/ActorFactory.h"
#include "scene/Script/CommonCommands.h"
#include "scene/Script/SpriteCommands.h"
#include "Script/ScriptClassParser.h"
#include "Graphic/Blender.h"
#include "Script/Class/ScriptAnimationClass.h"
#include "Script/Class/ScriptImageClass.h"
#include "Graphic/Renderer/TextureRenderer.h"

using namespace MX;





ScriptableSpriteActor::ScriptableSpriteActor(const ScriptableSpriteActor& other) : script(other.script)
{
	geometry = other.geometry;
}

ScriptableSpriteActor::ScriptableSpriteActor(LScriptObject& script) : script(script)
{
	script.load_property(geometry, "Geometry");

}


void ScriptableSpriteActor::Run()
{
	auto guard = Context<ScriptableSpriteActor>::Lock(this);
	script.onRun();
	SpriteActor::Run();
}

void ScriptableSpriteActor::Unlink()
{
	auto guard = Context<ScriptableSpriteActor>::Lock(this);
	if (linked())
		script.onUnlink();
	SpriteActor::Unlink();
}

void ScriptableSpriteActor::OnLinkedToScene()
{
	auto guard = Context<ScriptableSpriteActor>::Lock(this);
	script.onLink();
	SpriteActor::OnLinkedToScene();
}


SpriteActorPtr ScriptableSpriteActor::clone()
{
	return cloneSprite();
}

std::shared_ptr<ScriptableSpriteActor> ScriptableSpriteActor::cloneSprite()
{
	return std::make_shared<ScriptableSpriteActor>(*this);
}


BaseGraphicSceneScriptable::BaseGraphicSceneScriptable(const BaseGraphicSceneScriptable& other) : ScriptableSpriteActor(other)
{
	other.CloneChildrenTo(*this);
}

BaseGraphicSceneScriptable::BaseGraphicSceneScriptable(LScriptObject& script) : ScriptableSpriteActor(script)
{

	std::list<SpriteActorPtr> children;
	script.load_property(children, "Children");

	for (auto &actor : children)
		AddActor(actor);
}

void BaseGraphicSceneScriptable::Run()
{
	auto guard = Context<ScriptableSpriteActor>::Lock(this);
	script.onRun();
	BaseGraphicScene::Run();
}
void BaseGraphicSceneScriptable::Draw(float x, float y)
{
	auto guard = Context<ScriptableSpriteActor>::Lock(this); //TODO is that necessary?
	BaseGraphicScene::Draw(x,y);
}

void BaseGraphicSceneScriptable::Unlink()
{
	auto guard = Context<ScriptableSpriteActor>::Lock(this);
	if (linked())
		script.onUnlink();
	BaseGraphicScene::Unlink();
}

void BaseGraphicSceneScriptable::OnLinkedToScene()
{
	auto guard = Context<ScriptableSpriteActor>::Lock(this);
	script.onLink();
	BaseGraphicScene::OnLinkedToScene();
}

void BaseGraphicTransformSceneScriptable::Draw(float x, float y)
{
	if (_actors.empty())
		return;
	Graphic::TextureRenderer::current().Flush();

	auto color_guard = Context<Color, Graphic::Image::Settings::ColorMultiplier>::CreateEmptyGuard();

	if (geometry.color.current() != MX::Color::white())
		color_guard.Reset(geometry.color.current());

#ifdef WIP
	ci::gl::pushModelView();
	ci::gl::translate(x, y);
	ci::gl::rotate(geometry.angle, 0.0f, 0.0f, 1.0f);
	ci::gl::scale(geometry.scale.x, geometry.scale.y);
	ci::gl::translate(-x, -y);
#endif

	auto guard = Context<ScriptableSpriteActor>::Lock(this); //TODO is that necessary?
	BaseGraphicScene::Draw(x, y);
#ifdef WIP
	Graphic::TextureRenderer::current().Flush();
	ci::gl::popModelView();
#endif	
}

void BaseGraphicTransformSceneScriptable::DrawCustom(float x, float y)
{
	if (_actors.empty())
		return;
#ifdef WIP
	Graphic::TextureRenderer::current().Flush();
	ci::gl::pushModelView();
	ci::gl::translate(x, y);
	ci::gl::rotate(geometry.angle, 0.0f, 0.0f, 1.0f);
	ci::gl::scale(geometry.scale.x, geometry.scale.y);
	ci::gl::translate(-x, -y);
#endif

	auto guard = Context<ScriptableSpriteActor>::Lock(this); //TODO is that necessary?
	BaseGraphicScene::DrawCustom(x, y);

#ifdef WIP
	Graphic::TextureRenderer::current().Flush();
	ci::gl::popModelView();
#endif
}



ScAnimatedSpriteActor::ScAnimatedSpriteActor(const ScAnimatedSpriteActor& other) : AnimatedSpriteAdapter<ScriptableSpriteActor>(other)
{
	_animation = other._animation->clone();
}

ScAnimatedSpriteActor::ScAnimatedSpriteActor(LScriptObject& script) : AnimatedSpriteAdapter<ScriptableSpriteActor>(script)
{
	Graphic::SingleAnimation::pointer single_animation;
	script.load_property(single_animation, "Animation");


	Graphic::Animation::pointer animation;
	if (single_animation)
		animation = Graphic::Animation::Create(single_animation);
	SetAnimation(animation);

}

ScAnimatedSpriteActor::ScAnimatedSpriteActor(const Graphic::Animation::pointer &animation) : AnimatedSpriteAdapter<ScriptableSpriteActor>(animation)
{
}







ScSingleRunAnimatedSpriteActor::ScSingleRunAnimatedSpriteActor(const ScSingleRunAnimatedSpriteActor& other) : SingleRunAnimatedSpriteAdapter<ScriptableSpriteActor>(other)
{
	_animation = other._animation->clone();
	_freeze_last_frame = other._freeze_last_frame;
}

ScSingleRunAnimatedSpriteActor::ScSingleRunAnimatedSpriteActor(LScriptObject& script) : SingleRunAnimatedSpriteAdapter<ScriptableSpriteActor>(script)
{
	Graphic::SingleAnimation::pointer single_animation;
	script.load_property(single_animation, "Animation");

	Graphic::Animation::pointer animation;
	if (single_animation)
		animation = std::make_shared<Graphic::AnimationSingleRun>(single_animation);
	_animation = animation;

}

ScSingleRunAnimatedSpriteActor::ScSingleRunAnimatedSpriteActor(const Graphic::Animation::pointer &animation, bool freeze_last_frame) : SingleRunAnimatedSpriteAdapter<ScriptableSpriteActor>(animation, freeze_last_frame)
{
}




ScImageSpriteActor::ScImageSpriteActor(const ScImageSpriteActor& other) : ImageSpriteAdapter<ScriptableSpriteActor>(other, nullptr)
{
	_image = other._image;
}

ScImageSpriteActor::ScImageSpriteActor(LScriptObject& script) : ImageSpriteAdapter<ScriptableSpriteActor>(script, nullptr)
{
	script.load_property(_image, "Image");
}

ScImageSpriteActor::ScImageSpriteActor(const std::shared_ptr<Graphic::Image> &image) : ImageSpriteAdapter<ScriptableSpriteActor>(image)
{
}






ScRandomImageSpriteActor::ScRandomImageSpriteActor(const ScRandomImageSpriteActor& other) : ImageSpriteAdapter<ScriptableSpriteActor>(other, nullptr)
{
	SetAnimation(other._animation);
}

ScRandomImageSpriteActor::ScRandomImageSpriteActor(LScriptObject& script) : ImageSpriteAdapter<ScriptableSpriteActor>(script, nullptr)
{
	Graphic::SingleAnimation::pointer single_animation;
	script.load_property(single_animation, "Animation");
	SetAnimation(single_animation);
}

ScRandomImageSpriteActor::ScRandomImageSpriteActor(const Graphic::SingleAnimation::pointer &animation) : ImageSpriteAdapter<ScriptableSpriteActor>(nullptr)
{
	SetAnimation(animation);
}

void ScRandomImageSpriteActor::SetAnimation(const Graphic::SingleAnimation::pointer &animation)
{
	_animation = animation;
	if (!_animation || _animation->frames().empty())
		return;
	SetImage(_animation->random_frame().image);
}



void ScriptableSpriteActorInit::Init()
{
	CommonCommandsInit::Init();
	ScriptableCommandsInit::Init();
	ActorGeneratorInit::Init();

	
	//"On.Link"

	ScriptClassParser::AddCreator(L"Sprite.Scene.Transform", new OutsideScriptClassCreatorContructor<BaseGraphicTransformSceneScriptable>());
	ScriptClassParser::AddCreator(L"Sprite.Scene", new OutsideScriptClassCreatorContructor<BaseGraphicSceneScriptable>());
	ScriptClassParser::AddCreator(L"Sprite.Image", new OutsideScriptClassCreatorContructor<ScImageSpriteActor>());
	ScriptClassParser::AddCreator(L"Sprite.RandomImage", new OutsideScriptClassCreatorContructor<ScRandomImageSpriteActor>());
	ScriptClassParser::AddCreator(L"Sprite.Animation", new OutsideScriptClassCreatorContructor<ScAnimatedSpriteActor>());
	ScriptClassParser::AddCreator(L"Sprite.SingleRunAnimation", new OutsideScriptClassCreatorContructor<ScSingleRunAnimatedSpriteActor>());
	
}