#include "Widget.h"
#include "application/Window.h"
#include "Game/Resources/Resources.h"
#include <iostream>
#include <cmath>

#include "graphic/Renderer/TextureRenderer.h"

using namespace MX;
using namespace MX::Widgets;

WidgetTransform::WidgetTransform()
{

}

bool WidgetTransform::isIdentity()
{
	return _identity;
}

void WidgetTransform::SetTransform(const glm::mat4 &transform)
{
	_transform = transform;
	_identity = _transform == glm::mat4();
}

bool WidgetTransform::absoluteIsIdentity()
{
	return _absIdentity;
}
void WidgetTransform::SetParentTransform(const glm::mat4 &transform)
{
	_absoluteTransform = transform * _transform;
	_absIdentity = _absoluteTransform == glm::mat4();
}





bool MX::Widgets::Widget::drawDebug = false;

void MX::Widgets::Widget::AddActor(const MX::SpriteActorPtr &actor)
{
}

MX::Widgets::Widget::Widget() : _shapePolicy(*this)
{
	
}


//----children & parent
void MX::Widgets::Widget::AddWidget(const std::shared_ptr<Widget> &widget)
{
	_subWidgets.push_back(widget);
	LinkActor(widget);

    if (widget->_zValue != 0)
        onChildZOrderChanged();
}


void MX::Widgets::Widget::Clear()
{
	SpriteScene::Clear();
	_subWidgets.clear();
}


MX::StrongList<MX::Widgets::Widget>& MX::Widgets::Widget::widgets()
{
	return _subWidgets;
}

void MX::Widgets::Widget::for_each_child(const std::function<void(const std::shared_ptr<SpriteActor>&)> &functor)
{
	auto guard = _subWidgets.guard();
	auto it = _subWidgets.begin();
	while (it != _subWidgets.end())
	{
		functor(*it);
		_subWidgets.temp_advance(it);
	}
}

Widget *Widget::parentWidget() const
{
	return _parentWidget;
}


//---- Run & Draw
void MX::Widgets::Widget::InternalsOnRun()
{
	script.onRun();
#ifdef _DEBUG
	if (_drawer && _drawer->version() != _drawerVersion)
	{
		auto drawer = _drawer;
		SetDrawer(nullptr);
		SetDrawer(drawer);
	}

#endif

	for (auto &strategy : _runnableStrategies)
		strategy->Run();
}
void MX::Widgets::Widget::ChildrenOnRun()
{
	auto guard = _subWidgets.guard();
	auto it = _subWidgets.begin();
	while (it != _subWidgets.end())
	{
		(*it)->Run();
		_subWidgets.temp_advance(it);
	}
}

void MX::Widgets::Widget::Run()
{
	if (!visible())
		return;

	auto guard1 = Context<Widget>::Lock(this);
	//if you add something here, remember to add it to possible subclasses, or refactor it
	InternalsOnRun();
	ChildrenOnRun();
}

void MX::Widgets::Widget::Draw(float x, float y)
{
	if (!visible())
		return;

	//TODO make this optional, or rework #WIDGETFLOAT
	if (true)
	{
		x = floor(x);
		y = floor(y);
	}

	Drawer::ContextData contextData;
#ifdef _DEBUG
    contextData.drawDebugDrawers = drawDebug;
#endif

	auto guardc = Context<Drawer::ContextData>::Lock(contextData);
	auto guard1 = Context<Widget>::Lock(this);
	auto dest = Drawer::Destination( Rectangle::fromWH( x, y, Width(), Height() ) );
	auto guard2 = Context<Drawer::Destination>::Lock(dest);

	//TODO remove this, let widget handle this
	if (Drawer::isDragged() && !Drawer::drawDragged())
		return;

#ifdef WIPMATRIX
	if (drawDebug)
	{
		Graphic::TextureRenderer::current().Flush();
		ci::gl::pushModelView();
		ci::gl::setModelMatrix(glm::mat4());

		_shapePolicy.DebugDraw();
		ci::gl::popModelView();
		Graphic::TextureRenderer::current().Flush();
	}
		

	if (!_transform.isIdentity())
	{
		Graphic::TextureRenderer::current().Flush();
		ci::gl::pushModelView();
		ci::gl::multModelMatrix(_transform.transform());
	}
#endif

	for (auto &strategy : _drawableStrategies)
		strategy->BeforeDraw();

	DrawBackground();

	if (contextData.drawChildren)
	{
		DrawChildren();
	}

	DrawForeground();

	for (auto reverse_it = _drawableStrategies.rbegin(); reverse_it != _drawableStrategies.rend(); reverse_it++)
	{
		auto &strategy = *reverse_it;
		strategy->AfterDraw();
	}
		
#ifdef WIPMATRIX
	if (!_transform.isIdentity())
	{
		Graphic::TextureRenderer::current().Flush();
		ci::gl::popModelView();
	}
#endif
}

void MX::Widgets::Widget::DrawBackground()
{
	if (_drawer)
		_drawer->DrawBackground();
}

void MX::Widgets::Widget::DrawChildren()
{
	auto guard = _subWidgets.guard();
	auto it = _subWidgets.begin();
	while (it != _subWidgets.end())
	{
		(*it)->Draw((*it)->geometry.position.x, (*it)->geometry.position.y);
		_subWidgets.temp_advance(it);
	}
}

void MX::Widgets::Widget::DrawForeground()
{
	if (_drawer)
		_drawer->DrawForeground();
}

//properties, drawers, policies, strategies
void MX::Widgets::Widget::AddStrategy(const Strategy::Strategy::pointer& strategy)
{ 
	strategy->InitStrategy(*this); 
	_strategies.push_back(strategy); 
	if (strategy->runnable()) 
		_runnableStrategies.push_back(strategy.get()); 
	if (strategy->drawable())
		_drawableStrategies.push_back(strategy.get());
}

void MX::Widgets::Widget::AddStrategy(Strategy::Strategy& strategy)
{
	strategy.InitStrategy(*this);
	_staticStrategies.push_back(&strategy);
	if (strategy.runnable())
		_runnableStrategies.push_back(&strategy);
	if (strategy.drawable())
		_drawableStrategies.push_back(&strategy);
}

void MX::Widgets::Widget::SetDrawer(const Drawer::pointer& drawer)
{
	if (_drawer == drawer)
		return;
	_drawer = drawer;
	

	if (!drawer)
	{
		ChangeShapePolicy(nullptr);
		return;
	}

#ifdef _DEBUG
	_drawerVersion = _drawer->version();
#endif

	ChangeShapePolicy(_drawer->defaultShapePolicy());
	

	if (_drawer->defaultMargins())
		SetMargins(_drawer->defaultMargins().value());
	if (_width == 0)
	{
		for (auto &strategy : _drawableStrategies)
			strategy->BeforeDraw();


		auto size = _drawer->defaultSize();
		if (size)
			SetSize(size->x, size->y);

        for (auto reverse_it = _drawableStrategies.rbegin(); reverse_it != _drawableStrategies.rend(); reverse_it++)
        {
            auto &strategy = *reverse_it;
            strategy->AfterDraw();
        }
	}
		


	float width = _width, height = _height;
	clipSize(width, height);
	SetSize(width, height);


	{
		auto guard = Context<Widget>::Lock(this);
		drawer->OnAssignedToWidget();

		IterateOverStrategies([](auto &s) { s.OnRefreshedDrawer(); });
	}

}


void MX::Widgets::Widget::ChangeShapePolicy(std::unique_ptr<ShapePolicy>&& newPolicy)
{
	if (!_shapePolicy.Change(std::move(newPolicy), *this))
		return;
	IterateOverStrategies([](auto &s) { s.OnShapeChanged(); });
}

void MX::Widgets::Widget::SetTransform(const glm::mat4 &transform)
{
	_transform.SetTransform(transform);
	onParentTransformMatrixChanged(parentTransformMatrix());
}

void MX::Widgets::Widget::SetMargins(const MX::Margins &margins)
{
	auto delta = margins.topLeft() - _margins.topLeft();

	_margins = margins;

	moveChildren(delta);
	OnAreaInsideChanged();
}

void MX::Widgets::Widget::SetVerticalScroll(float y)
{ 
	auto delta = _scroll.y - y;
	_scroll.y = y; 
	moveChildren({ 0.0f, delta });
}
void MX::Widgets::Widget::SetHorizontalScroll(float x)
{ 
	auto delta = _scroll.x - x;
	_scroll.x = x; 
	moveChildren({ delta, 0.0f });
}


glm::vec2 MX::Widgets::Widget::relativePosition()
{
	return linked() ? geometry.position - sprite_scene().geometry.position : geometry.position;
}


glm::vec2 MX::Widgets::Widget::position()
{
	auto npos = geometry.position;
	auto parent = parentWidget();
	if (parent)
	{
		npos -= parentWidgetOffset();
	}
	return npos;
}

glm::vec2 MX::Widgets::Widget::minDimensions()
{
	glm::vec2 v(.0f, .0f);
	clipSize(v.x, v.y);
	return v;
}
glm::vec2 MX::Widgets::Widget::maxDimensions()
{
	glm::vec2 v(1000000.0f, 1000000.0f);
	clipSize(v.x, v.y);
	return v;
}

bool MX::Widgets::Widget::SetPosition(const glm::vec2& position)
{
	glm::vec2 npos = position;
	npos += parentWidgetOffset();
    round(npos);

	auto inf = std::numeric_limits<float>::infinity();
	if (position.x == inf)
		npos.x = geometry.position.x;
	if (position.y == inf)
		npos.y = geometry.position.y;

	if (npos == geometry.position)
		return false;

	auto delta = npos - geometry.position;

	geometry.position = npos;
	_shapePolicy.OnMoved(*this);

	
	moveChildren(delta);
	return true;
}


void MX::Widgets::Widget::onParentMove(const glm::vec2& delta)
{
	geometry.position += delta;
	_shapePolicy.OnMoved(*this);
	moveChildren(delta);
}


void MX::Widgets::Widget::onParentTransformMatrixChanged(const glm::mat4 &transform)
{
	_transform.SetParentTransform(transform);
	auto t = absoluteTransformMatrix();
	_shapePolicy.OnTransformMatrixChanged(*this, t);

	auto guard = _subWidgets.guard();
	auto it = _subWidgets.begin();
	while (it != _subWidgets.end())
	{
		(*it)->onParentTransformMatrixChanged(t);
		_subWidgets.temp_advance(it);
	}
}

glm::mat4 MX::Widgets::Widget::absoluteTransformMatrix()
{
#if 0
	auto parent = parentWidget();
	if (!parent)
		return _transform.transform();

	return parent->_transform.transform() * _transform.transform(); //TODO or possibly other way...
#endif
	return _transform.absoluteTransform();
}

glm::mat4 MX::Widgets::Widget::parentTransformMatrix()
{
	auto parent = parentWidget();
	if (!parent)
		return glm::mat4();

	return parent->_transform.absoluteTransform();
}

void MX::Widgets::Widget::SetPosition(float x, float y, float width, float height)
{
	clipSize(width, height);

	if (_width == width && _height == height)
	{
		SetPosition(x, y);
		return;
	}

	_width = std::round(width);
	_height = std::round(height);
	if (!SetPosition(x, y))
		_shapePolicy.OnMoved(*this);
	OnSizeChanged();
	
}

void MX::Widgets::Widget::SetSize(float width, float height)
{
	clipSize(width, height);

	if (_width == width && _height == height)
		return;

    _width = std::round(width);
    _height = std::round(height);
	_shapePolicy.OnMoved(*this);
	OnSizeChanged();
}

void MX::Widgets::Widget::onChildZOrderChanged()
{
    auto comparer = [](auto &a, auto &b)
    {
        int va = a ? a->_zValue : 0;
        int vb = b ? b->_zValue : 0;
        return va < vb;
    };

    _subWidgets.sort(comparer);
}

void MX::Widgets::Widget::clipSize(float &width, float &height)
{
	if (_drawer)
	{
		for (auto &strategy : _drawableStrategies)
			strategy->BeforeDraw();
		_drawer->clipSize(width, height);
        for (auto reverse_it = _drawableStrategies.rbegin(); reverse_it != _drawableStrategies.rend(); reverse_it++)
        {
            auto &strategy = *reverse_it;
            strategy->AfterDraw();
        }
	}
		
}

void MX::Widgets::Widget::OnSizeChanged()
{
	on_size_changed(_width, _height);
	OnAreaInsideChanged();
}

void MX::Widgets::Widget::OnUnlinkedFromScene()
{
	auto parent = parentWidget();
	if (parent)
	{
		_parentWidget->onChildRemoved(*this);
		_parentWidget = nullptr;
	}

	onParentMove(-parentWidgetOffset());
	_shapePolicy.ChangeState(*this);
}

void MX::Widgets::Widget::OnLinkedToScene()
{
	_parentWidget = dynamic_cast<Widget *>(_scene);
	auto parent = parentWidget();
	if (parent)
	{
		auto delta = parentWidgetOffset();
		onParentMove(delta);

		//TODO optimize we could lazy calc this
		onParentTransformMatrixChanged(parentTransformMatrix());
	}
	
	_shapePolicy.ChangeState(*this);
    
}

void MX::Widgets::Widget::onBecameVisible()
{
	_shapePolicy.ChangeState(*this);
}

void MX::Widgets::Widget::onBecameInvisible()
{
	_shapePolicy.ChangeState(*this);
}







glm::vec2 MX::Widgets::Widget::parentWidgetOffset()
{
	auto parent = parentWidget();
	if (parent)
		return parent->geometry.position + parent->_margins.topLeft() - parent->_scroll;
	return {};
}

void MX::Widgets::Widget::moveChildren(const glm::vec2& delta)
{
	auto guard = _subWidgets.guard();
	auto it = _subWidgets.begin();
	while (it != _subWidgets.end())
	{
		(*it)->onParentMove(delta);
		_subWidgets.temp_advance(it);
	}
}

void MX::Widgets::Widget::IterateOverStrategies(const std::function<void(Strategy::Strategy&)>& f)
{
	for (auto &s : _strategies)
		f(*s);

	for (auto &s : _staticStrategies)
		f(*s);
}








ButtonWidget::ButtonWidget()
{

}

bool ButtonWidget::isState(State state)
{
	std::function<bool(void)> states[] = { 
						[&](){ return _enabled; },
						[&](){ return _hover; },
						[&](){ return _pressed; },
						[&](){ return _selected; },
					};

	return states[(int)state]();

}


void ButtonWidget::SetPressed(bool pressed)
{
	if (_pressed == pressed)
		return;
	_pressed = pressed;

	if (!enabled())
		return;
	if (_pressed)
		onTouched();
	else
		onReleased();
}

void ButtonWidget::SetSelected(bool selected)
{
	if (_selected == selected)
		return;
	_selected = selected;
	if (!enabled())
		return;
	if (_selected)
		onSelected();
	else
		onUnselected();
	onSelectedChanged(selected);
}

void ButtonWidget::SetHover(bool hover)
{
	if (_hover == hover)
		return;
	_hover = hover;
	if (!enabled())
		return;
	if (_hover)
		onHoverIn();
	else
		onHoverOut();
}


void ButtonWidget::SetEnabled(bool enabled)
{
	if (_enabled == enabled)
		return;
	_enabled = enabled;
	onEnabledChanged(_enabled);
}

bool ButtonWidget::pressed()
{
	return _pressed;
}

bool ButtonWidget::selected()
{
	return _selected;
}

bool ButtonWidget::enabled()
{
	return _enabled;
}

bool ButtonWidget::hover()
{
	return _hover;
}




WidgetBackground::WidgetBackground()
{
	static Drawer::pointer drawer = 0;
	SetDrawer(_drawer);
}


void WidgetHolder::SetContent(const std::shared_ptr<Widget> &widget)
{
	if (_widget)
		_widget->Unlink();
	if (widget)
	{
		_widget = widget;

		if (_forwardContentSize)
			onChildSizeChanged();

		auto d = dimensionsInside();
		_widget->SetPosition(0.0f, 0.0f, d.x, d.y);
		Widget::AddWidget(widget);
	}
	onContentChanged(widget);
}

void WidgetHolder::OnAreaInsideChanged()
{
	if (_widget && !_forwardContentSize)
	{
		auto d = dimensionsInside();
		_widget->SetPosition(0.0f, 0.0f, d.x, d.y);
	}
}

void WidgetHolder::clipSize(float &width, float &height)
{
	Widget::clipSize(width, height);

	if (!_forwardContentSize)
		return;

	if (_widget)
		_widget->clipSize(width, height);
}

void WidgetHolder::onChildSizeChanged()
{
	Widget::onChildSizeChanged();
	if (!_forwardContentSize)
		return;

	float width = Width();
	float height = Height();
	clipSize(width, height);

	if (width != Width() || height != Height())
	{
		SetSize(width, height);
		NotifyParentAboutSizeUpdate();
	}
}

void WidgetHolder::SetButtonLinkedWithContent(const std::shared_ptr<ButtonWidget>& button, const std::shared_ptr<Widget> &widget)
{
	if (button)
	{
		auto f = [b = button.get(), widget]
			(auto &cw)
		{
			b->SetSelected(widget == cw);
		};
		if (widget)
			onContentChanged.connect(f, button);

		auto holder = MX::weaken(shared_to<WidgetHolder>());
		button->onClicked.connect([widget, h = std::move(holder)]()
		{
			auto holder = h.lock();
			holder->SetContent(widget);
		}, shared_from_this());
	}


	if (widget && !_widget)
		SetContent(widget);
}