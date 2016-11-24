#include "Scene.h"
#include "utils/Utils.h"
#include "utils/Singleton.h"
#include <iostream>
using namespace std;

namespace MX
{
    Scene::~Scene()
    {
        
    }


	void Scene::LinkActor(const Actor::pointer &actor)
	{
		actor->_scene = this;
		actor->OnLinkedToScene();
		LinkedActor(actor);
	}

	BaseScene::BaseScene()
	{
	
	}

	void BaseScene::AddActor(const Actor::pointer &actor)
	{
		_actors.push_back(actor);
		LinkActor(actor);
		
	}

	void BaseScene::AddActorAtFront(const Actor::pointer &actor)
	{
		_actors.push_front(actor);
		LinkActor(actor);
		
	}



	void BaseScene::Run()
	{
		auto guard = _actors.guard();
		auto it = _actors.begin();
		while (it != _actors.end())
		{
			(*it)->Run();
			_actors.temp_advance(it);	
		}

	}

	const StrongList<Actor> &BaseScene::actors() const
	{
		return _actors;
	}






	void InfoScene::Run()
	{
		bool interrupted = false;
		SceneInfo info;
		{
			auto guardContext = Context<SceneInfo>::Lock(info);

			auto guard = _actors.guard();
			auto it = _actors.begin();
			while (it != _actors.end())
			{
				(*it)->Run();
				if (info.interrupt)
				{
					interrupted = true; 
					break;
				}
				_actors.temp_advance(it);	
			}
		}

		if (Context<SceneInfo>::isCurrent())
			Context<SceneInfo>::current().interrupt = interrupted;
	}

	bool InfoScene::RunInterrupted()
	{
		SceneInfo info;
		auto guard = Context<SceneInfo>::Lock(info);
		Run();
		return info.interrupt;
	}

}