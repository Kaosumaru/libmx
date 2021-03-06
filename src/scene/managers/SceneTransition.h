#pragma once
#include "graphic/images/TextureImage.h"
#include "scene/sprites/SpriteScene.h"
#include "utils/Time.h"
#include <list>

namespace MX
{

class SceneTransition : public virtual SpriteActor
{
public:
    virtual void SetScenes(const std::shared_ptr<SpriteScene>& old_scene, const std::shared_ptr<SpriteScene>& new_scene) { }
};

class BaseBitmapTransition : public virtual SceneTransition
{
public:
    BaseBitmapTransition(float time = 0.5f);
    void SetScenes(const std::shared_ptr<SpriteScene>& old_scene, const std::shared_ptr<SpriteScene>& new_scene) override;
    void Run() override;
    void Draw(float x, float y) override;

protected:
    Graphic::TextureImage::pointer _scene1;
    Graphic::TextureImage::pointer _scene2;
    Time::ManualStopWatch _stopWatch;
    double _time;
};

class AlphaBitmapTransition : public virtual BaseBitmapTransition
{
public:
    AlphaBitmapTransition(float time = 0.5f);
    void Draw(float x, float y) override;
};

class MoveBitmapTransition : public virtual BaseBitmapTransition
{
public:
    MoveBitmapTransition(bool left = true, float time = 0.5f);
    void Draw(float x, float y) override;

protected:
    bool _left;
};

}
