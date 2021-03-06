#include "ScriptLayoutersCommon.h"
#include "ScriptLayouters.h"
#include "application/Window.h"
#include "game/resources/Resources.h"
#include "script/PropertyLoaders.h"
#include "script/ScriptClassParser.h"
#include <iostream>

using namespace MX::Widgets;

class ScriptLayouterList : public ScriptLayouterBase
{
public:
    ScriptLayouterList(MX::LScriptObject& script)
        : ScriptLayouterBase(script)
    {
        script.load_property(_spacing, "Spacing");
        script.load_property(_fill, "Fill");
    }

    void OnLinkedToWidget() override
    {
        ScriptLayouterBase::OnLinkedToWidget();
    }

    void LayoutWidget(const std::string& name, const std::shared_ptr<Widget>& widget) override
    {
        Context<ScriptLayouterWidget>::current().InvalidateLayout();
    }

    void RelayoutAllWidgets(const NamedWidgetsMap& named_widgets) override
    {
        FindDrawersForAllWidgets(named_widgets);

        auto d = CalculateSize(named_widgets, [](const std::shared_ptr<Widget>& widget, MX::Rectangle& r) {
            widget->SetPositionRect(r);
        });

        if (_fill)
        {
            if (_horizontal)
                d.y = -1;
            else
                d.x = -1;
        }

        ChangeInnerDimension(d);
    }

    void clipSize(float& width, float& height) override
    {
        //TODO it would be nifty to cache this,
        //I think after relayout this will be always called
        //perhaps we can now (if widget isn't invalidated, return it's size)
        auto& w = Context<ScriptLayouterWidget>::current();
        auto d = CalculateSize(w.namedWidgets(), [](const std::shared_ptr<Widget>& widget, MX::Rectangle& r) {

        });

        if (!(_fill && !_horizontal))
            width = d.x + w.margins().hMargins();
        if (!(_fill && _horizontal))
            height = d.y + w.margins().vMargins();
    }

protected:
    template <typename T>
    glm::vec2 CalculateSize(const NamedWidgetsMap& named_widgets, const T& f)
    {
        float secondaryDimensionMax = 0.0f;
        float mainDimensionOffset = 0.f;
        bool first = true;
        for (auto& pair : named_widgets)
        {
            WidgetLayoutHint hint;
            auto l = Context<WidgetLayoutHint>::Lock(hint);
            if (_fill)
            {
                auto dim = _horizontal ? Context<ScriptLayouterWidget>::current().dimensionsInside().y : Context<ScriptLayouterWidget>::current().dimensionsInside().x;
                if (dim < 0)
                    dim = 0;
                if (_horizontal)
                    hint.suggested_height = dim;
                else
                    hint.suggested_width = dim;
            }

            if (first)
                first = false;
            else
                mainDimensionOffset += _spacing;

            auto& widget = pair.second;
            auto minSize = widget->minDimensions();

            auto w = minSize.x;
            auto h = minSize.y;

            if (_fill)
            {
                if (_horizontal)
                    h = Context<ScriptLayouterWidget>::current().dimensionsInside().y;
                else
                    w = Context<ScriptLayouterWidget>::current().dimensionsInside().x;
            }

            MX::Rectangle r = MX::Rectangle::fromWH(_horizontal ? mainDimensionOffset : 0.0f, _horizontal ? 0.0f : mainDimensionOffset, w, h);
            f(widget, r);

            mainDimensionOffset += _horizontal ? widget->Width() : widget->Height();
            //TODO fix for fill
            secondaryDimensionMax = std::max(secondaryDimensionMax, _horizontal ? minSize.y : minSize.x);
        }
        if (_horizontal)
            return { mainDimensionOffset, secondaryDimensionMax };
        else
            return { secondaryDimensionMax, mainDimensionOffset };
    }

    bool _horizontal = true;
    float _spacing = 0;
    bool _fill = false;
};

class ScriptLayouterListHorizontal : public ScriptLayouterList
{
public:
    ScriptLayouterListHorizontal(MX::LScriptObject& script)
        : ScriptLayouterList(script)
    {
        _horizontal = true;
    }
};

class ScriptLayouterListVertical : public ScriptLayouterList
{
public:
    ScriptLayouterListVertical(MX::LScriptObject& script)
        : ScriptLayouterList(script)
    {
        _horizontal = false;
    }
};

class ScriptLayouterGrid : public ScriptLayouterBase
{
public:
    ScriptLayouterGrid(MX::LScriptObject& script)
        : ScriptLayouterBase(script)
    {
        script.load_property(_itemSize, "Item.Size");
        script.load_property(_itemSpacing, "Item.Spacing");
    }

    void LayoutWidget(const std::string& name, const std::shared_ptr<Widget>& widget) override
    {
        Context<ScriptLayouterWidget>::current().InvalidateLayout();
    }

    void RelayoutAllWidgets(const NamedWidgetsMap& named_widgets) override
    {
        FindDrawersForAllWidgets(named_widgets);

        if (!Drawer::Destination::isCurrent())
        {
            assert(false);
            return;
        }

        auto& currentDrawer = Drawer::Destination::current();

        int width = currentDrawer.width();
        int itemsPerRow = (int)(width + _itemSpacing.x) / ((int)_itemSize.x + _itemSpacing.x);
        itemsPerRow = itemsPerRow > 0 ? itemsPerRow : 1;

        float startX = (width - itemsPerRow * _itemSize.x - (itemsPerRow - 1) * _itemSpacing.x) / 2;
        float startY = 0.0f;

        float x = startX;
        float y = startY;

        int rowIndex = 0;

        for (auto& pair : named_widgets)
        {
            auto& widget = pair.second;
            auto minSize = widget->minDimensions();

            auto currentCell = MX::Rectangle::fromWH(x, y, _itemSize.x, _itemSize.y);

            widget->SetPositionRect(currentCell);

            x += _itemSize.x + _itemSpacing.x;

            rowIndex++;

            if (rowIndex == itemsPerRow)
            {
                rowIndex = 0;
                x = startX;
                y += _itemSize.y + _itemSpacing.y;
            }
        }

        //currently we don't change size of GridLayouter
        //ChangeInnerDimension(d);
    }

    void clipSize(float& width, float& height) override
    {
        ScriptLayouterBase::clipSize(width, height);
    }

protected:
    glm::vec2 _itemSize;
    glm::vec2 _itemSpacing;
};

void ScriptLayouterCommonInit::Init()
{
    ScriptClassParser::AddCreator(L"Layouter.Horizontal", new OutsideScriptClassCreatorContructor<ScriptLayouterListHorizontal>());
    ScriptClassParser::AddCreator(L"Layouter.Vertical", new OutsideScriptClassCreatorContructor<ScriptLayouterListVertical>());
    ScriptClassParser::AddCreator(L"Layouter.Grid", new OutsideScriptClassCreatorContructor<ScriptLayouterGrid>());
}
