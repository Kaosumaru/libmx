#ifndef MXWIDGET
#define MXWIDGET
#include "utils/Utils.h"
#include "utils/Vector2.h"
#include "Drawers/Drawer.h"
#include "scene/sprites/SpriteScene.h"
#include "Collision/Shape/SignalizingShape.h"
#include "Strategies/Strategy.h"
#include "utils/Signal.h"
#include "Drawers/DrawerManager.h"
#include "Shapes/Shapes.h"
#include "scene/Script/ScriptableActor.h"

namespace MX
{
namespace Widgets
{

class Widget;







class PropertyMap
{
public:

	template<typename T>
	void SetValue(const std::string &name, const T& v) { /*_values[name] = v;*/ }

	template<typename T>
	T valueOf(const std::string &name, const T& def = {}) const
	{
		/*
		auto it = _values.find(name);
		if (it == _values.end())
			return def;
		return boost::get<T>(it->second);*/
		return{};
	}

protected:
	//using Type = stx::variant < float, std::string >;
	//std::unordered_map<std::string, Type> _values;
};


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


class Widget : public virtual SpriteScene, public ScopeSingleton<Widget>, public shared_ptr_init<Widget>
{
	friend class Strategy::Strategy;
	friend class Drawer;
	friend class ScriptLayouter;
public:
	Widget();
	std::shared_ptr<Actor> cloneActor() { return clone(); }

	virtual void AddWidget(const std::shared_ptr<Widget> &widget);
	void Clear() override;
	void for_each_child(const std::function<void(const std::shared_ptr<SpriteActor>&)> &functor) override;
	StrongList<Widget>& widgets();
	Widget *parentWidget() const;

	void Run() override;
	void Draw(float x, float y) override;


	glm::vec2 position();
	glm::vec2 relativePosition(); //position relative to parent
	float Width() override { return _width; }
	float Height() override { return _height; }
	glm::vec2 dimensions() { return{ (float)Width(), (float)Height() }; }
	glm::vec2 dimensionsInside() { return{ (float)Width() - _margins.hMargins(), (float)Height() - _margins.vMargins() }; }

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

	
	void AddStrategy(const Strategy::Strategy::pointer& strategy);
	void SetDrawer(const Drawer::pointer& drawer);
	void ChangeShapePolicy(std::unique_ptr<ShapePolicy>&& newPolicy);

	void SetMargins(const MX::Margins &margins);
	const MX::Margins &margins() { return _margins; }
	
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

	void onBecameVisible();
	void onBecameInvisible();

	void AddActor(const SpriteActorPtr &actor);
	void onParentMove(const glm::vec2& delta);

	void onParentTransformMatrixChanged(const glm::mat4 &transform);
	

	void OnLinkedToScene();
	void OnUnlinkedFromScene();

	float _width = 0.0f;
	float _height = 0.0f;
    int   _zValue = 0;

	std::shared_ptr<Drawer> _drawer;
#ifdef _DEBUG
	int                     _drawerVersion = 0;
	int                     _debugID = -1;
#endif

	StrongList<Widget> _subWidgets;
	std::vector<Strategy::Strategy::pointer> _strategies;
	std::vector<Strategy::Strategy*> _runnableStrategies;
	std::vector<Strategy::Strategy*> _drawableStrategies;
	std::vector<Strategy::Strategy*> _staticStrategies;

	MX::Margins _margins;
	glm::vec2   _scroll;

	PropertyMap _properties;

	WidgetTransform       _transform;
	ShapePolicyController _shapePolicy;

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
