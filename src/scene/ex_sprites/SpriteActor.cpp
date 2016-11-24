#include "MXSpriteActor.h"
#include <iostream>
#include "Utils/MXTime.h"
#include "Scene/Sprites/MXSpriteScene.h"
using namespace MX;


#ifdef _DEBUG
int SpriteActor::_liveCounter = 0;
#endif


SpriteActor::SpriteActor()
{
#ifdef _DEBUG
	_liveCounter++;
#endif
	geometry.scale = Vector2(1.0f, 1.0f);
	geometry.angle = 0.0f;
}

SpriteActor::~SpriteActor()
{
#ifdef _DEBUG
	_liveCounter--;
#endif
}

SpriteScene &SpriteActor::sprite_scene()
{
    return *(static_cast<SpriteScene *>(_scene));
}

MX::Vector2 SpriteActor::absolute_position()
{
	auto position = geometry.position;
	if (_scene)
		sprite_scene().translate_child_position(position);
	return position;
}



