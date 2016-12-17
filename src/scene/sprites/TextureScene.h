#pragma once

#include "SpriteScene.h"
#include "ScrollableSpriteScene.h"
#include "graphic/images/TextureImage.h"
#include <list>

namespace MX
{


class TextureDisplaySceneTimer : public DisplaySceneTimer
{
public:
	TextureDisplaySceneTimer(const glm::vec2& size);
	void Draw(float x, float y) override;
	float Width();
	float Height();
protected:
	Graphic::TextureImage::pointer _texture;
};

class TextureScene : public virtual BaseGraphicScene
{
public:
	TextureScene(const glm::vec2& size, const Color &backgroundColor = 0x00000000, bool alpha = false);
	void Draw(float x, float y) override;
	float Width();
	float Height();

	const Graphic::TextureImage::pointer& texture() { return _texture; }
protected:
	Graphic::TextureImage::pointer _texture;
	Color _backgroundColor;
};

class ScrollableTextureScene : public TextureScene, public ScrollableSpriteScene
{
public:
	ScrollableTextureScene(const glm::vec2 &viewDimensions, const glm::vec2 &sceneDimensions);
	void Draw(float x = 0.0f, float y = 0.0f) override;
	void Run() override;

	float Width();
	float Height();
};

}