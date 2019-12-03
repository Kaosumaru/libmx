#pragma once
#include "SceneManager.h"
#include "utils/Signal.h"
#include <stack>

namespace MX
{

class SpriteSceneStackManager : public BaseSpriteSceneManager
{
public:
    SpriteSceneStackManager();

    void SelectScene(const SpriteScenePtr& scene, const std::shared_ptr<SceneTransition>& transition = nullptr) override;
    void PushScene(const std::shared_ptr<SpriteScene>& scene, const std::shared_ptr<SceneTransition>& transition = nullptr);
    void PopScene(const std::shared_ptr<SceneTransition>& transition = nullptr);

    void Run() override { BaseSpriteSceneManager::Run(); }
    void Draw(float x = 0.0f, float y = 0.0f) override { BaseSpriteSceneManager::Draw(x, y); }

    static SpriteSceneStackManager* manager_of(SpriteActor* actor);
    static void PopOnManagerOf(SpriteActor* actor)
    {
        auto manager = SpriteSceneStackManager::manager_of(actor);
        if (manager)
            manager->PopScene();
    }

    void Clear() override;

    MX::Signal<void(void)> onPush;
    MX::Signal<void(void)> onPop;
    static Signal<void(SpriteSceneStackManager*)> onCreated;

    const auto& scenesStack() { return _scenesStack; }

private:
    std::stack<SpriteScenePtr> _scenesStack;
};

}
