#pragma once
#include<memory>
#include "scene/sprites/SpriteScene.h"
#include "scene/managers/SceneManager.h"
#include "utils/Utils.h"

namespace MX
{
    struct CheatObject
    {
        virtual ~CheatObject() {}
    };

    std::shared_ptr<CheatObject> CreateCheats();
    
}
