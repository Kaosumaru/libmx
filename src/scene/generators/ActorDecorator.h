#pragma once
#include "ActorCreator.h"

namespace MX{

class ActorDecorator : public ScriptObject, public shared_ptr_init<ActorDecorator>
{
public:
	ActorDecorator(){}
	ActorDecorator(LScriptObject& script)
	{
		
	}

	virtual void DecorateActor(const std::shared_ptr<ScriptableSpriteActor> &actor)
	{
		
	}
	virtual ActorDecorator::pointer clone() { return nullptr; }
};



class NotifierActorDecorator : public MX::ActorDecorator
{
public:
	using Function = std::function<void(const std::shared_ptr<MX::ScriptableSpriteActor> &actor)>;

	NotifierActorDecorator(const Function& f) : _f(f)
	{

	}

	void DecorateActor(const std::shared_ptr<MX::ScriptableSpriteActor> &actor) override
	{
		if (_f)
			_f(actor);
	}
protected:
	Function _f;

};

class ActorDecoratorScripts : public ActorDecorator
{
public:
	ActorDecoratorScripts(){}
	ActorDecoratorScripts(LScriptObject& script) : ActorDecorator(script)
	{
		script.load_property(onRun, "On.Run");
		script.load_property(onLink, "On.Link");
		script.load_property(onUnlink, "On.Unlink");
	}

	void DecorateActor(const std::shared_ptr<ScriptableSpriteActor> &actor)
	{
		actor->script.onRun += onRun;
		actor->script.onLink += onLink;
		actor->script.onUnlink += onUnlink;
	}
	ActorDecorator::pointer clone() 
	{
		auto col = std::make_shared<ActorDecoratorScripts>();
		col->onRun += onRun;
		col->onLink += onLink;
		col->onUnlink += onUnlink;
		return col;
	}

	CommandSignal onRun;
	ScriptSignal onLink;
	ScriptSignal onUnlink;
};



class ActorDecoratorGeometry : public ActorDecorator
{
public:
	ActorDecoratorGeometry(){}
	ActorDecoratorGeometry(LScriptObject& script) : ActorDecorator(script)
	{
		script.load_property(geometry, "Geometry");
	}

	void DecorateActor(const std::shared_ptr<ScriptableSpriteActor> &actor)
	{
		actor->geometry.color = geometry.color;
	}
	ActorDecorator::pointer clone() override { return std::make_shared<ActorDecoratorGeometry>(*this); }

	MX::SpriteActor::Geometry geometry;
};


class ActorDecoratorPositionInFactory : public ActorDecorator
{
public:
	ActorDecoratorPositionInFactory(){ _offset = 0.0f; }
	ActorDecoratorPositionInFactory(LScriptObject& script) : ActorDecorator(script)
	{
		script.load_property(_offset, "Offset");
	}

	void DecorateActor(const std::shared_ptr<ScriptableSpriteActor> &actor);

	ActorDecorator::pointer clone() override { return std::make_shared<ActorDecoratorPositionInFactory>(*this); }

	MX::SpriteActor::Geometry geometry;
protected:
	float _offset = 0.0f;
};




}
