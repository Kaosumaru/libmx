#include "TextureScene.h"
#include "Application/Window.h"

using namespace std;
using namespace MX;

TextureDisplaySceneTimer::TextureDisplaySceneTimer(const glm::vec2& size) : DisplaySceneTimer(size)
{
	_texture = Graphic::TextureImage::Create(size.x, size.y);
}
void TextureDisplaySceneTimer::Draw(float x, float y)
{
	if (!visible())
		return;
	{
		Graphic::TargetContext context(*_texture);
		DisplayScene::Draw(0.0f,0.0f);
	}
	_texture->DrawCentered( {}, { x, y }, geometry.scale, geometry.angle, geometry.color );
}
float TextureDisplaySceneTimer::Width()
{
	return _texture->Width();
}
float TextureDisplaySceneTimer::Height()
{
	return _texture->Height();
}


TextureScene::TextureScene(const glm::vec2& size, const Color &backgroundColor, bool alpha) : _backgroundColor(backgroundColor)
{
	_texture = Graphic::TextureImage::Create(size.x, size.y, alpha);
}

void TextureScene::Draw(float x, float y)
{
	if (!visible())
		return;
	{
		Graphic::TargetContext context(*_texture);
		_texture->Clear(_backgroundColor);
		BaseGraphicScene::Draw(0.0f,0.0f);
	}
	_texture->DrawCentered( {}, { x, y }, geometry.scale, geometry.angle, geometry.color );
}

float TextureScene::Width()
{
	return _texture->Width();
}
float TextureScene::Height()
{
	return _texture->Height();
}


ScrollableTextureScene::ScrollableTextureScene(const glm::vec2& viewDimensions, const glm::vec2& sceneDimensions) : TextureScene(viewDimensions), ScrollableSpriteScene(viewDimensions, sceneDimensions)
{
}

void ScrollableTextureScene::Run()
{
	TextureScene::Run();
	ScrollableSpriteScene::Run();
}

void ScrollableTextureScene::Draw(float x, float y)
{
	if (!visible())
		return;
	{
		Graphic::TargetContext context(*_texture);
		_texture->Clear(_backgroundColor);
		BaseGraphicScene::Draw((int)-_cameraPosition.x, (int)-_cameraPosition.y);
	}
	_texture->DrawCentered( {}, { x, y}, geometry.scale, geometry.angle, geometry.color );
}


float ScrollableTextureScene::Width()
{
	return _viewDimensions.x;
}

float ScrollableTextureScene::Height()
{
	return _viewDimensions.x;
}