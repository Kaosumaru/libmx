#ifndef MXWIDGET
#define MXWIDGET
#include "utils/Utils.h"
#include "utils/Vector2.h"
#include "drawers/Drawer.h"
#include "scene/sprites/SpriteScene.h"
#include "collision/shape/SignalizingShape.h"
#include "strategies/Strategy.h"
#include "utils/Signal.h"
#include "drawers/DrawerManager.h"
#include "shapes/Shapes.h"
#include "scene/script/ScriptableActor.h"
#include "PropertyMap.h"

namespace MX
{
namespace Widgets
{

class Widget;

class WidgetTransform
{
public:
	WidgetTransform();

	bool isIdentity();
	void SetTransform(const glm::mat4 &transform);
	const auto& transform() { return _transform; }

	bool absoluteIsIdentity();
	void SetParentTransform(const glm::mat4 &transform);
	const auto& absoluteTransform() { return _absoluteTransform; }
protected:
	bool _identity = true;
	bool _absIdentity = true;
	glm::mat4 _transform;
	glm::mat4 _absoluteTransform; //transform stacked on parent transform
};


class Widget : public SpriteScene, public ScopeSingleton<Widget>, public shared_ptr_init<Widget>
{
	friend class Strategy::Strategy;
	friend class Drawer;
	friend class ScriptLayouter;
public:
	Widget();
	std::shared_ptr<Actor> cloneActor() override { return clone(); }

	virtual void AddWidget(const std::shared_ptr<Widget> &widget);
	void Clear() override;
	void for_each_child(const std::function<void(const std::shared_ptr<SpriteActor>&)> &functor) override;
	StrongList<Widget>& widgets();
	Widget *parentWidget() const;

	void Run() override;
	void Draw(float x, float y) override;


	glm::vec2 position();
	glm::vec2 relativePosition(); //position relative to parent
	float Width() override { return _dimensions.x; }
	float Height() override { return _dimensions.y; }
	glm::vec2 dimensions() { return _dimensions; }
	glm::vec2 dimensionsInside() { return{ Width() - _margins.hMargins(), Height() - _margins.vMargins() }; }

	virtual glm::vec2 minDimensions();
	virtual glm::vec2 maxDimensions();

	void ShiftPosition(const glm::vec2& delta) { SetPosition(position() + delta); }

	bool SetPosition(float x, float y) { return SetPosition({ x, y }); }
	bool SetPosition(const glm::vec2& position);
	void SetPosition(float x, float y, float width, float height); //position relative to parent
	void SetPositionRect(const MX::Rectangle& r) { SetPosition(r.x1, r.y1, r.width(), r.height()); }

	void SetSize(float width, float height);

	void SetVerticalScroll(float y);
	void SetHorizontalScroll(float x);
	const glm::vec2& scroll() { return _scroll; }
	MX::Rectangle scrollViewport() { return MX::Rectangle::fromWH(scroll().x, scroll().y, dimensionsInside().x, dimensionsInside().y); }

	
	void AddStrategy(const Strategy::Strategy::pointer& strategy);
	void SetDrawer(const Drawer::pointer& drawer);
	void ChangeShapePolicy(std::unique_ptr<ShapePolicy>&& newPolicy);

	void SetMargins(const MX::Margins &margins);
	const MX::Margins &margins() { return _margins; }

	void ResetContentBounds() { _contentBounds = MX::Rectangle {}; }
	void SetContentBounds(const MX::Rectangle &bounds) { _contentBounds = bounds; }
	const MX::Rectangle &contentBounds() { return _contentBounds; }
	
	const auto& shape() { return _shapePolicy.shape(); }
	PropertyMap &properties() { return _properties; }
	bool interactive() { return _requestedInteraction > 0; } //if it's interactive, then it should have shape

	Signal<void(float width, float height)> on_size_changed; //WIP check unlinking


	const auto& transform() { return _transform.transform(); }
	void SetTransform(const glm::mat4 &transform);
	glm::mat4 absoluteTransformMatrix();
	glm::mat4 parentTransformMatrix();

	virtual void clipSize(float &width, float &height);

	static bool drawDebug;

	ScriptData script;

    void SetZValue(int value)
    {
        _zValue = value;
        auto parent = parentWidget();
        if (parent)
            parent->onChildZOrderChanged();
    }

#ifdef _DEBUG
	void SetDebugID(int id) { _debugID = id; }
#endif
protected:
	void InternalsOnRun();
	void ChildrenOnRun();

	void AddStrategy(Strategy::Strategy& strategy);

	virtual void DrawBackground();
	void DrawChildren();
	void DrawForeground();

	virtual void IterateOverStrategies(const std::function<void(Strategy::Strategy&)>& f);

	glm::vec2 parentWidgetOffset();

	void NotifyParentAboutSizeUpdate()
	{
		auto parent = parentWidget();
		if (parent)
			parent->onChildSizeChanged();
	}

	void IncInteractive()
	{
		if (_requestedInteraction++ == 0)
			OnInteractiveChanged();
	}

	void DecInteractive()
	{
		if (--_requestedInteraction == 0)
			OnInteractiveChanged();
	}


	void OnInteractiveChanged()
	{
		_shapePolicy.ChangeState(*this);
	}

    virtual void onChildZOrderChanged();
	virtual void OnSizeChanged();
	virtual void OnAreaInsideChanged(){};
	virtual void onChildSizeChanged() {}
	virtual void onChildRemoved(Widget& child) {}

	void translate_child_position(glm::vec2 & position) override {};

	void moveChildren(const glm::vec2& delta);

	void onBecameVisible() override;
	void onBecameInvisible() override;

	void AddActor(const SpriteActorPtr &actor) override;
	void onParentMove(const glm::vec2& delta);

	void onParentTransformMatrixChanged(const glm::mat4 &transform);
	

	void OnLinkedToScene() override;
	void OnUnlinkedFromScene() override;

	glm::vec2 _dimensions;
    int   _zValue = 0;

	std::shared_ptr<Drawer> _drawer; // current drawer - object/script which handles visual representation of this widget
#ifdef _DEBUG
	int                     _drawerVersion = 0;
	int                     _debugID = -1;
#endif

	StrongList<Widget> _subWidgets;  // children
	std::vector<Strategy::Strategy::pointer> _strategies;  // strategies - components providing logic (autoscroll, etc) or data (text, images for drawers)
	std::vector<Strategy::Strategy*> _runnableStrategies;
	std::vector<Strategy::Strategy*> _drawableStrategies;
	std::vector<Strategy::Strategy*> _staticStrategies;

	MX::Margins     _margins;        // _margins - interpreted by layouter (and helper dimensionsInside)
	glm::vec2       _scroll;         // scroll offset
	MX::Rectangle   _contentBounds; // calculated by layouter!

	PropertyMap _properties;

	WidgetTransform       _transform;   // transformation matrix
	ShapePolicyController _shapePolicy; // transformation matrix

	int                   _requestedInteraction = 0;

	Widget                *_parentWidget = nullptr;
};

class ButtonWidget : public Widget
{
public:
	ButtonWidget();

	virtual void SetSelected(bool selected);
	virtual void SetEnabled(bool enabled);
	virtual void SetPressed(bool pressed);
	virtual void SetHover(bool hover);

	bool pressed();
	bool selected();
	bool enabled();
	bool hover();

	enum class State
	{
		Enabled = 0,
		Hover,
		Pressed,
		Selected
	};

	bool isState(State state);

	Signal<void(bool)> onEnabledChanged;

    Signal<void(void)> onTouched;
    Signal<void(void)> onReleased;
	Signal<void(void)> onClicked;
	Signal<void(bool)> onSelectedChanged;
	Signal<void(void)> onSelected;
	Signal<void(void)> onUnselected;
	Signal<void(void)> onHoverIn;
	Signal<void(void)> onHoverOut;
protected:
	bool                            _hover = false;
	bool                            _enabled = true;
	bool                            _pressed = false;
	bool                            _selected = false;
};



class WidgetHolder : public Widget
{
public:
	WidgetHolder(bool forwardSize = false) : _forwardContentSize(forwardSize) {}

	void SetContent(const std::shared_ptr<Widget> &widget);
	void SetButtonLinkedWithContent(const std::shared_ptr<ButtonWidget>& button, const std::shared_ptr<Widget> &widget);
	bool IsEmpty() const { return _widget == nullptr; }

	MX::Signal<void(const std::shared_ptr<Widget>&)> onContentChanged;
protected:
	void OnAreaInsideChanged() override;
	void AddWidget(const std::shared_ptr<Widget> &widget) override {};

	void clipSize(float &width, float &height) override;
	void onChildSizeChanged() override;

	std::shared_ptr<Widget>	_widget;
	bool _forwardContentSize = false;
};

//TODO 
class WidgetBackground : public Widget
{
public:
	WidgetBackground();
};



}

using WidgetPtr = std::shared_ptr<Widgets::Widget>;

}

#endif
