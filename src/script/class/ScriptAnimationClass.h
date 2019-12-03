#pragma once
#include "ScriptImageClass.h"
#include "graphic/animation/SingleAnimation.h"
#include "script/ScriptClass.h"
#include "utils/Utils.h"
#include <string>

namespace MX
{

class ScriptAnimationClass : public ScriptImageClass
{
public:
    ScriptAnimationClass();

    virtual const std::shared_ptr<Graphic::SingleAnimation>& animation() const;
    std::shared_ptr<Graphic::Image> image() const;

protected:
    std::shared_ptr<Graphic::SingleAnimation> _animation;
};

class ScriptAnimationFromFileClass : public ScriptAnimationClass
{
public:
    ScriptAnimationFromFileClass();

protected:
    bool onParse() override;
};

class ScriptAnimationFromFilesClass : public ScriptAnimationClass
{
public:
    ScriptAnimationFromFilesClass();

protected:
    bool onParse() override;

    float _frameDuration = 0.0f;
    unsigned _framesCount = 0;
    std::string _filenameTemplate;
    glm::vec2 _pivot;
};

class ScriptRandomAnimation : public ScriptAnimationClass
{
public:
    ScriptRandomAnimation();

    const std::shared_ptr<Graphic::SingleAnimation>& animation() const override;

protected:
    bool onParse() override;

    std::vector<std::shared_ptr<Graphic::SingleAnimation>> _animations;
};

}
