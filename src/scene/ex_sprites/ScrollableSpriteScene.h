#ifndef MXSCROLLABLESPRITESCENE
#define MXSCROLLABLESPRITESCENE

#include "MXSpriteScene.h"


namespace MX
{

class ScrollableSpriteScene : public virtual SpriteScene
{
public:
	ScrollableSpriteScene(const Vector2 &viewDimensions, const Vector2 &sceneDimensions);

	void Run();

	unsigned Width();
	unsigned Height();

	const MX::Vector2& cameraPosition() { return _cameraPosition; }
	void SetCameraPosition(const Vector2& position);
	void centerCameraOn(const Vector2& position);
	Vector2 screenCenter();
	Vector2 cameraOffsetSinceLastRun() { return _cameraPosition - _cameraPositionOnLastRun; }

	MX::Vector2 from_screen_point(const MX::Vector2& screen_point);
	MX::Vector2 to_screen_point(const MX::Vector2& arena_point);

    
    float scale() { return _scale; }
    void SetScale(float scale) { _scale = scale; }
protected:
    float viewWidth();
    float viewHeight();
    
	Rectangle absoluteSceneBounds();

	bool isViewLargerThanScene() { return viewWidth() > _sceneDimensions.x || viewHeight() > _sceneDimensions.y; }
	Vector2 _viewDimensions;
	Vector2 _sceneDimensions;
	Vector2 _cameraPosition;
	Vector2 _cameraPositionOnLastRun;
    float   _scale = 1.0f;

};

//main scene
class ScrollableBaseGraphicScene : public BaseGraphicScene, public ScrollableSpriteScene
{
public:
	ScrollableBaseGraphicScene(const Vector2 &viewDimensions, const Vector2 &sceneDimensions);
	void Draw(float x = 0.0f, float y = 0.0f);
	void Run();
};

class ScrollableBitmapScene : public BitmapScene, public ScrollableSpriteScene
{
public:
	ScrollableBitmapScene(const Vector2 &viewDimensions, const Vector2 &sceneDimensions);
	void Draw(float x = 0.0f, float y = 0.0f);
	void Run();

	unsigned Width();
	unsigned Height();
};

}

#endif
