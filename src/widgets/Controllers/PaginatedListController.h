#pragma once
#include "Widgets/Widget.h"
#include "ListController.h"
#include "Widgets/Strategies/Strategy.h"
#include "utils/SignalizingVariable.h"

namespace MX
{
namespace Widgets
{

struct PaginationControl
{
	PaginationControl() {}

	SignalizingVariable<int> pages = 1;
	SignalizingVariable<int> currentPage = 0;

	MX::Signal<void(void)> onViewChanged;

	void NextPage()
	{
		SetPage(currentPage + 1);
	}

	void PreviousPage()
	{
		SetPage(currentPage - 1);
	}

	void SetPage(int p)
	{
		if (p >= pages)
			p = pages - 1;
		if (p < 0)
			p = 0;
		if (p == currentPage)
			return;

		currentPage = p;
		onViewChanged();
	}

	void SetMaxItemsPerPage(int max)
	{
		if (_maxItemsPerPage == max)
			return;
		_maxItemsPerPage = max;
		Recalculate();
		onViewChanged();
	}

	void SetItems(int count)
	{
		if (_items == count)
			return;
		_items = count;
		Recalculate();
		onViewChanged();
	}

	template<typename T>
	std::vector<T> copyClipped(const std::vector<T>& arr)
	{
		std::vector<T> clipped;
		clipped.reserve(_maxItemsPerPage);

		auto firstIndex = indexOfFirstVisibleItem();
		auto copyCount = _maxItemsPerPage;

        if (firstIndex < (int)arr.size())
        {
            int lastToCopy = firstIndex + copyCount - 1;
            if (lastToCopy >= (int)arr.size())
                copyCount = arr.size() - firstIndex;

            std::copy_n(arr.begin() + firstIndex, copyCount, std::back_inserter(clipped));
        }
        else
            copyCount = 0;

		if (_fillPageToEnd && copyCount < _maxItemsPerPage)
            clipped.resize(_maxItemsPerPage);

		return clipped;
	}

	int indexOfFirstVisibleItem()
	{
		return currentPage * _maxItemsPerPage;
	}

	auto maxItemsPerPage() { return _maxItemsPerPage; }

	void SetFillPageToEnd(bool fillToEnd)
	{
		_fillPageToEnd = fillToEnd;
		Recalculate();
		onViewChanged();
	}
protected:
	void Recalculate()
	{
		pages = (_items + _maxItemsPerPage - 1) / _maxItemsPerPage; //divide, rounded up
		if (currentPage != 0 && currentPage >= pages)
			currentPage = pages - 1;
	}

	bool _fillPageToEnd = false;
	int _maxItemsPerPage = 3;
	int _items = 0;
};

template<typename T, typename Type = OneSelectionListController<T> >
class PaginatedListController : public Strategies::Strategy
{
public:
	using pointer = std::shared_ptr<PaginatedListController>;
	using CreatorFunction = typename Type::CreatorFunction;
	using ContainerType = typename Type::ContainerType;
	using ControllerType = Type;
    using ItemContext = typename Type::ItemContext;

	PaginatedListController(const CreatorFunction& function) : _controller(function)
	{
		pagination().onViewChanged.connect([&]() { if (_settingData) return;  SetData(_data); }); //update view after pagination change
	}

	static std::shared_ptr<PaginatedListController> Create(const CreatorFunction& function)
	{
		return MX::make_shared< PaginatedListController<T, ControllerType> >(function);
	}

	void SetData(const ContainerType &data)
	{
		_settingData = true;
		_data = data;
		_pagination.SetItems(data.size());

		auto clipped = _pagination.copyClipped(data);
		_controller.SetData(clipped);
		_settingData = false;
	}

	auto &controller()
	{
		return _controller;
	}

	auto &pagination()
	{
		return _pagination;
	}

	void DecorateAsNextButton(const std::shared_ptr<ButtonWidget>& w, bool hideWhenOnePage = false)
	{
		w->onClicked.connect([&]() { _pagination.NextPage(); });

		auto onViewChanged = [=]()
		{
			if (hideWhenOnePage)
				w->SetVisible(!(pagination().pages > 1));
			bool enabled = pagination().pages > 1 && pagination().currentPage != pagination().pages - 1;
			w->SetEnabled(enabled);
		};

		pagination().onViewChanged.connect(onViewChanged);
		onViewChanged();
	}

	void DecorateAsPrevButton(const std::shared_ptr<ButtonWidget>& w, bool hideWhenOnePage = false)
	{
		w->onClicked.connect([&]() { _pagination.PreviousPage(); });

		auto onViewChanged = [=]()
		{
			if (hideWhenOnePage)
				w->SetVisible(!(pagination().pages > 1));
			bool enabled = pagination().pages > 1 && pagination().currentPage != 0;
			w->SetEnabled(enabled);
		};

		pagination().onViewChanged.connect(onViewChanged);
		onViewChanged();
	}

	void OnRefreshedDrawer() override
	{
		auto items = widget().properties().valueOf<float>("ItemsPerPage", -1.0f);
		if (items != -1.0f)
			_pagination.SetMaxItemsPerPage(items);

		pagination().SetFillPageToEnd(widget().properties().valueOf<float>("FillPageToEnd", 0.0f) != 0.0f);
	}

protected:


	void OnInit() override
	{
		_controller.InitStrategy(widget());
	}

	
	bool              _settingData = false;

	ControllerType     _controller;
	PaginationControl _pagination;

	ContainerType _data;
};


}
}
