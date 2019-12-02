#include "SceneStackManager.h"
using namespace std;

using namespace MX;

Signal<void(SpriteSceneStackManager*)> SpriteSceneStackManager::onCreated;

SpriteSceneStackManager::SpriteSceneStackManager()
{
    onCreated(this);
}

void SpriteSceneStackManager::PushScene(const std::shared_ptr<SpriteScene>& scene, const std::shared_ptr<SceneTransition>& transition)
{
    _scenesStack.push(scene);
    SelectScene(scene, transition);
    onPush();
}

void SpriteSceneStackManager::PopScene(const std::shared_ptr<SceneTransition>& transition)
{
    if (_scenesStack.empty())
    {
        assert(false);
        return;
    }
    _scenesStack.pop();
    if (!_scenesStack.empty())
        SelectScene(_scenesStack.top(), transition);
    onPop();
}

SpriteSceneStackManager* SpriteSceneStackManager::manager_of(SpriteActor* actor)
{
    SpriteActor* act = actor;
    while (act->linked())
        act = &(act->sprite_scene());
    return dynamic_cast<SpriteSceneStackManager*>(act);
}

void SpriteSceneStackManager::SelectScene(const SpriteScenePtr& scene, const std::shared_ptr<SceneTransition>& transition)
{
    BaseSpriteSceneManager::SelectScene(scene, transition);
}

void SpriteSceneStackManager::Clear()
{
    BaseSpriteSceneManager::Clear();
    while (!_scenesStack.empty())
        _scenesStack.pop();
}
