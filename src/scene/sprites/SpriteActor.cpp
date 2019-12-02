#include "SpriteActor.h"
#include "scene/sprites/SpriteScene.h"
#include <iostream>
using namespace MX;

#ifdef _DEBUG
int SpriteActor::_liveCounter = 0;
#endif

SpriteActor::SpriteActor()
{
#ifdef _DEBUG
    _liveCounter++;
#endif
    geometry.scale = { 1.0f, 1.0f };
    geometry.angle = 0.0f;
}

SpriteActor::~SpriteActor()
{
#ifdef _DEBUG
    _liveCounter--;
#endif
}

SpriteScene& SpriteActor::sprite_scene()
{
    return *(static_cast<SpriteScene*>(_scene));
}

glm::vec2 SpriteActor::absolute_position()
{
    auto position = geometry.position;
    if (_scene)
        sprite_scene().translate_child_position(position);
    return position;
}
