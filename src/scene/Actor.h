#pragma once
#include <functional>
#include "utils/Utils.h"
#include "utils/StrongList.h"

namespace MX
{

class Scene;

class SceneLink
{
public:
	virtual ~SceneLink();
	virtual void disconnect();
};

class Actor : public virtual disable_copy_constructors, public Linkable
{
	friend class Scene;
public:
	typedef std::shared_ptr<Actor> pointer;
	typedef std::weak_ptr<Actor> weak_pointer;

	Actor();
	virtual ~Actor();

	virtual void Run() { };

	virtual void Unlink();

	virtual Scene &scene();
	
	bool linked();

	virtual std::shared_ptr<Actor> cloneActor() { return nullptr; }

	static pointer fromFunctor(const std::function<void (void)> &func);

	
	virtual bool empty() { return true; }
protected:
	virtual void OnLinkedToScene();
	virtual void OnUnlinkedFromScene();

	Scene *_scene;
};

}

