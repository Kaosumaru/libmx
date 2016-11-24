#include "MXScrollableSpriteScene.h"


using namespace std;
using namespace MX;

ScrollableSpriteScene::ScrollableSpriteScene(const Vector2 &viewDimensions, const Vector2 &sceneDimensions)
{
	_viewDimensions = viewDimensions;
	_sceneDimensions = sceneDimensions;
}

void ScrollableSpriteScene::Run()
{
	_cameraPositionOnLastRun = _cameraPosition;
}

unsigned ScrollableSpriteScene::Width()
{
	return _viewDimensions.x;
}

unsigned ScrollableSpriteScene::Height()
{
	return _viewDimensions.y;
}

float ScrollableSpriteScene::viewWidth()
{
    return _viewDimensions.x / _scale;
}

float ScrollableSpriteScene::viewHeight()
{
    return _viewDimensions.y / _scale;
}

void ScrollableSpriteScene::SetCameraPosition(const Vector2& position)
{
    auto vWidth = viewWidth();
    auto vHeight = viewHeight();
    
    
	_cameraPosition = position;
	_cameraPosition.clampX(0.0f, _sceneDimensions.x - vWidth);
	_cameraPosition.clampY(0.0f, _sceneDimensions.y - vHeight);

	if (_sceneDimensions.x < vWidth)
		_cameraPosition.x = -(vWidth - _sceneDimensions.x) / 2.0f;
	if (_sceneDimensions.y < vHeight)
		_cameraPosition.y = -(vHeight - _sceneDimensions.y) / 2.0f;

}

MX::Rectangle ScrollableSpriteScene::absoluteSceneBounds()
{
	MX::Rectangle r = MX::Rectangle::fromWH(-_cameraPosition.x * _scale, -_cameraPosition.y * _scale, _sceneDimensions.x * _scale, _sceneDimensions.y * _scale);

	return r.intersection(MX::Rectangle({ 0.0f, 0.0f }, _viewDimensions));
}


void ScrollableSpriteScene::centerCameraOn(const Vector2& position)
{
	SetCameraPosition(position - screenCenter());
}

Vector2 ScrollableSpriteScene::screenCenter()
{
	return (_viewDimensions / _scale) /2.0f;
}


MX::Vector2 ScrollableSpriteScene::from_screen_point(const MX::Vector2& outside)
{
	auto inside = outside / _scale;
	inside += _cameraPosition;
	return inside;
}
MX::Vector2 ScrollableSpriteScene::to_screen_point(const MX::Vector2& inside)
{
	auto outside = inside;
	outside -= _cameraPosition;
	return outside * _scale;
}


ScrollableBaseGraphicScene::ScrollableBaseGraphicScene(const Vector2 &viewDimensions, const Vector2 &sceneDimensions) : ScrollableSpriteScene(viewDimensions, sceneDimensions)
{

}

void ScrollableBaseGraphicScene::Run()
{
	BaseGraphicScene::Run();
	ScrollableSpriteScene::Run();
}

void ScrollableBaseGraphicScene::Draw(float x, float y)
{
	BaseGraphicScene::Draw(x - _cameraPosition.x, y - _cameraPosition.y);
}




ScrollableBitmapScene::ScrollableBitmapScene(const Vector2 &viewDimensions, const Vector2 &sceneDimensions) : BitmapScene(viewDimensions), ScrollableSpriteScene(viewDimensions, sceneDimensions)
{
}

void ScrollableBitmapScene::Run()
{
	BitmapScene::Run();
	ScrollableSpriteScene::Run();
}

void ScrollableBitmapScene::Draw(float x, float y)
{
	if (!visible())
		return;
	{
		Graphic::TargetContext context(*_bitmap);
		_bitmap->Clear(_backgroundColor);
		BaseGraphicScene::Draw((int)-_cameraPosition.x, (int)-_cameraPosition.y);
	}
	_bitmap->DrawCentered(0.0f, 0.0f, x, y, geometry.scale.x, geometry.scale.y, geometry.angle, geometry.color);
}


unsigned ScrollableBitmapScene::Width()
{
	return _viewDimensions.x;
}

unsigned ScrollableBitmapScene::Height()
{
	return _viewDimensions.x;
}
