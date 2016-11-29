#pragma once
#include "Widgets/Widget.h"
#include "Widgets/Strategies/Strategy.h"

namespace MX
{
namespace Widgets
{

//populates parent with controls created from data
template<typename T, typename SignalType = default_signal<void (const T&)> >
class ListController : public Strategies::Strategy, public shared_ptr_init<ListController<T, SignalType>>
{
public:
	~ListController()
	{
		//for (auto &view : _views)
		//	view->Unlink();
	}

	typedef std::function< std::shared_ptr<Widget> (const T&, ListController* controller, unsigned int index) > CreatorFunction;
	SignalType OnAction;

	static std::shared_ptr<ListController> Create(const CreatorFunction& function)
	{
		return MX::make_shared< ListController<T, SignalType> > (function);
	}

	ListController(const CreatorFunction& function)
	{
		_creator = function;
	}

	virtual void SetData(const std::list<T> &data)
	{
		for (auto &view : _views)
			view->Unlink();
		_views.clear();
		_data = data;
		_views.resize(_data.size());
		auto it2 = _views.begin();

		unsigned int index = 0;
		for (auto &dataItem : _data)
		{
			auto widget = _creator(dataItem, this, index);
			*it2 = widget;
			this->widget().AddWidget(widget);
			it2 ++;
			index ++;
		}
	}

protected:
	CreatorFunction _creator;
	std::list<std::shared_ptr<Widget>> _views;
	std::list<T> _data;
};


// automatically listen for touched
template< typename T, template <class Created, class Alloc = std::allocator<Created> > class Container = std::vector>
class OneSelectionListController : public Strategies::Strategy, public shared_ptr_init<OneSelectionListController<T, Container >>
{
public:
	using ContainerType = Container<T> ;

	~OneSelectionListController()
	{
		//for (auto &view : _views)
		//	view->Unlink();
	}

    struct ItemContext
    {
        bool default_item = false;
    };

	typedef default_signal<void (const T&, int index)> SignalType;
	typedef std::function< std::shared_ptr<ButtonWidget> (const T&, OneSelectionListController* controller, unsigned int index, ItemContext &ctx) > CreatorFunction;
	SignalType OnAction;
	default_signal<void()> OnListEmpty;

	static std::shared_ptr<OneSelectionListController> Create(const CreatorFunction& function)
	{
		return MX::make_shared< OneSelectionListController<T, Container> >(function);
	}

	OneSelectionListController(const CreatorFunction& function)
	{
		_creator = function;
	}

	void SetData(const ContainerType &data)
	{
		_currentWidget = nullptr;
		for (auto &view : _views)
			view->Unlink();
		_views.clear();
		_data = data;
		_views.resize(_data.size());
		auto it2 = _views.begin();
		unsigned int index = 0;
		for (auto &dataItem : _data)
		{
            ItemContext ctx;
			auto widget = _creator(dataItem, this, index, ctx);
			*it2 = widget;

			this->widget().AddWidget(widget);
			
			if (_currentWidget == nullptr || ctx.default_item)
				SelectedButton(widget, dataItem, index);

			widget->onTouched.connect(boost::bind(&OneSelectionListController::SelectedButton, this, widget, boost::ref(dataItem), index));

			it2 ++;
			index ++;
		}

		if (!_currentWidget)
			OnListEmpty();
	}

	void SelectedButton(const std::shared_ptr<ButtonWidget>& widget, const T& data, int index)
	{
		if (_currentWidget)
			_currentWidget->SetSelected(false);
		_currentWidget = widget;
		_currentWidget->SetSelected(true);
		OnAction(data, index);
	}

	void SelectedIndex(int index)
	{
		if (index < 0 || index >= (int)_data.size())
			return;
		auto &widget = _views[index];
		auto &data = _data[index];
		SelectedButton(widget, data, index);
	}

	const auto& data() { return _data; }

protected:
	std::shared_ptr<ButtonWidget> _currentWidget;
	CreatorFunction _creator;
	std::vector<std::shared_ptr<ButtonWidget>> _views;
	ContainerType _data;
};






}
}

