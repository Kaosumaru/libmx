#ifndef MXSIMPLELAYOUTERS
#define MXSIMPLELAYOUTERS
#include "widgets/Widget.h"
namespace MX
{
namespace Widgets
{


class LayouterWidget;

class Layouter : public shared_ptr_init<Layouter>
{
public:
	Layouter();
	virtual ~Layouter(){};

	virtual void LayoutNewItem(const std::shared_ptr<Widget> &widget) = 0;
	virtual void LayoutAllItems() = 0;

	virtual void ItemRemoved(const std::shared_ptr<Widget> &widget) {}
	virtual void ItemResized(const std::shared_ptr<Widget> &widget) {}

	void SetMargins(const MX::Margins &margins);
	void SetInnerMargin(int inner);

protected:
	void SetSize(float width, float height);

	MX::Margins _margins;
	int _inner_margin;
};




class HorizontalLayouter : public Layouter
{
public:
	void LayoutNewItem(const std::shared_ptr<Widget> &widget) override;
	void LayoutAllItems() override;

	void ItemRemoved(const std::shared_ptr<Widget> &widget) override;
	void ItemResized(const std::shared_ptr<Widget> &widget) override;
};

class VerticalLayouter : public Layouter
{
public:
	void LayoutNewItem(const std::shared_ptr<Widget> &widget) override;
	void LayoutAllItems() override;

	void ItemRemoved(const std::shared_ptr<Widget> &widget) override;
	void ItemResized(const std::shared_ptr<Widget> &widget) override;
};


class LayouterWidget : public Widget
{
public:
	LayouterWidget();
	void AddWidget(const std::shared_ptr<Widget> &widget);
	void SetLayouter(const Layouter::pointer &layouter);
	Layouter::pointer &layouter();
protected:
	Layouter::pointer _layouter;
};


class VerticalLayouterWidget : public LayouterWidget
{
public:
	VerticalLayouterWidget(int inner = 0);
};


class HorizontalLayouterWidget : public LayouterWidget
{
public:
	HorizontalLayouterWidget(int inner = 0);
};


}
typedef std::shared_ptr<Widgets::LayouterWidget> LayouterWidgetPtr;
typedef std::shared_ptr<Widgets::VerticalLayouterWidget> VerticalLayouterWidgetPtr;

}

#endif
