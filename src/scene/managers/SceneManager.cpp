#include "SceneManager.h"
using namespace std;

namespace MX
{

SpriteSceneManager::SpriteSceneManager()
{
    _visibility = -1;
}

void SpriteSceneManager::LinkedActor(const Actor::pointer& actor)
{
    SpriteScene::LinkedActor(actor);
}
void SpriteSceneManager::UnlinkedActor(Actor& actor)
{
    SpriteScene::UnlinkedActor(actor);
}

void BaseSpriteSceneManager::SelectScene(const std::shared_ptr<SpriteScene>& scene, const std::shared_ptr<SceneTransition>& transition)
{
    if (_currentScene == scene)
        return;
    auto old_scene = _currentScene;

    _currentScene = scene;

    auto removeOld = [&]() {
        if (old_scene)
        {
            old_scene->SetVisible(false);
            old_scene->Unlink();
        }
    };

    if (transition)
    {
        _transitionScene = transition;
        _currentScene->ChangeVisibility(-1);
        _currentScene->Run(); //run once, since drawing may depend on running
        transition->SetScenes(old_scene, _currentScene);
        _currentScene->ChangeVisibility(1);
        removeOld();
        AddActor(transition);
        return;
    }

    removeOld();
    _currentScene->SetVisible(true);
    AddActor(_currentScene);
}

void BaseSpriteSceneManager::Run()
{
    std::shared_ptr<SpriteScene> scene = _currentScene;
    SpriteSceneManager::Run();

    if (_transitionScene && !_transitionScene->linked() && _currentScene)
    {
        _transitionScene = nullptr;
        _currentScene->SetVisible(true);
        AddActor(_currentScene);
    }

    if (!_transitionScene && scene)
    {
        if (!scene->linked() && scene == _currentScene)
            _currentScene = nullptr;
    }
}

void BaseSpriteSceneManager::Draw(float x, float y)
{
    if (_transitionScene)
        _transitionScene->Draw(x, y);
    else if (_currentScene)
        _currentScene->Draw(x, y);
}

void BaseSpriteSceneManager::Clear()
{
    SpriteSceneManager::Clear();
    _currentScene = nullptr;
    _transitionScene = nullptr;
}

}