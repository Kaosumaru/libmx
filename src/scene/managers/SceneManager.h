#pragma once
#include "Scene/Sprites/SpriteScene.h"
#include "SceneTransition.h"
#include <list>

namespace MX
{





class SpriteSceneManager : public BaseGraphicScene
{
public:
	SpriteSceneManager();
	void LinkedActor(const Actor::pointer &actor) override;
	void UnlinkedActor(Actor &actor) override;
};

//TODO add list!
class BaseSpriteSceneManager : public SpriteSceneManager
{
public:
	virtual void SelectScene(const SpriteScenePtr &scene, const std::shared_ptr<SceneTransition> &transition = nullptr);
	void Run();
	void Draw(float x = 0.0f, float y = 0.0f);

	void Clear() override;
protected:
	SpriteScenePtr _currentScene;
	std::shared_ptr<SceneTransition> _transitionScene;
};

}