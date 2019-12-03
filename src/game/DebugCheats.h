#pragma once
#include "scene/managers/SceneManager.h"
#include "scene/sprites/SpriteScene.h"
#include "utils/Utils.h"
#include <memory>

namespace MX
{
struct CheatObject
{
    virtual ~CheatObject() { }
};

std::shared_ptr<CheatObject> CreateCheats();

}
