#pragma once

#include "SpriteScene.h"

namespace MX
{

class ScrollableSpriteScene : public virtual SpriteScene
{
public:
    ScrollableSpriteScene(const glm::vec2& viewDimensions, const glm::vec2& sceneDimensions);

    void Run() override;

    float Width() override;
    float Height() override;

    const glm::vec2& cameraPosition() { return _cameraPosition; }
    void SetCameraPosition(const glm::vec2& position);
    void centerCameraOn(const glm::vec2& position);
    glm::vec2 screenCenter();
    glm::vec2 cameraOffsetSinceLastRun() { return _cameraPosition - _cameraPositionOnLastRun; }

    glm::vec2 from_screen_point(const glm::vec2& screen_point);
    glm::vec2 to_screen_point(const glm::vec2& arena_point);

    float scale() { return _scale; }
    void SetScale(float scale) { _scale = scale; }

protected:
    float viewWidth();
    float viewHeight();

    Rectangle absoluteSceneBounds();

    bool isViewLargerThanScene() { return viewWidth() > _sceneDimensions.x || viewHeight() > _sceneDimensions.y; }
    glm::vec2 _viewDimensions;
    glm::vec2 _sceneDimensions;
    glm::vec2 _cameraPosition;
    glm::vec2 _cameraPositionOnLastRun;
    float _scale = 1.0f;
};

//main scene
class ScrollableBaseGraphicScene : public BaseGraphicScene, public ScrollableSpriteScene
{
public:
    ScrollableBaseGraphicScene(const glm::vec2& viewDimensions, const glm::vec2& sceneDimensions);
    void Draw(float x = 0.0f, float y = 0.0f) override;
    void Run() override;
};

}
