#include "DestinationDrawers.h"
#include "widgets/Widget.h"
#include "script/ScriptClassParser.h"

using namespace MX;
using namespace MX::Widgets;



struct DestinationProperty
{
	friend struct PropertyLoader<DestinationProperty>;
	DestinationProperty(float v = 0.0f) : _value(v)
	{

	}

	float value()
	{
		return _value;
	}

	float value(float max, float offset = 0.0f) const
	{
		float result = _nonConstValue ? *_nonConstValue : _value;

		if (_percent)
			result *= max;

		if (result < 0.0f)
			result = max + result + 1.0f;

		if (_additive)
			result += offset;

		return result;
	}


	bool _percent = false;
	bool _additive = false;
	bool _loaded = false;

	float _value = 0.0f;
protected:
	Scriptable::Value::pointer _nonConstValue;
};

struct DestinationPropertyRect
{
	DestinationProperty x1, y1;
	DestinationProperty x2 = -1.0f, y2 = -1.0f;

	static Drawer::Destination destinationClip(const DestinationPropertyRect& rectangle)
	{
		if (Drawer::Destination::isCurrent())
		{
			//Drawer::Destination dest(rectangle);
			auto &current = Drawer::Destination::current();
			MX::Rectangle rect;

			rect.x1 = rectangle.x1.value(current.width());
			rect.x2 = rectangle.x2.value(current.width(), rect.x1);
			rect.y1 = rectangle.y1.value(current.height());
			rect.y2 = rectangle.y2.value(current.height(), rect.y1);

			rect.Shift(current.rectangle.pointTopLeft());
			return Drawer::Destination(rect);
		}
		return Drawer::Destination();
	}
};

namespace MX {

	template<>
	struct PropertyLoader<DestinationProperty>
	{
		using type = PropertyLoader_Standard;
		static bool load(DestinationProperty& out, const Scriptable::Value::pointer& obj)
		{
			if (!obj->member()->isConstant())
				out._nonConstValue = obj;
			else
				out._value = *obj;
			out._loaded = true;
			out._percent = obj->type() == Scriptable::ValueType::Percent;
			return true;
		}
	};

}

class DrawerWithChildren : public Drawer
{
public:
    DrawerWithChildren(MX::LScriptObject& script) : Drawer(script)
    {
        script.load_property(_children, "Children");
    }

protected:
    void DrawChildren()
    {
        for (auto &child : _children)
            child->DrawBackground();
    }


    std::vector<Drawer::pointer> _children;
};

class DestinationBaseDrawer : public DrawerWithChildren
{
protected:
	DestinationBaseDrawer(MX::LScriptObject& script, bool useWH = true) : DrawerWithChildren(script)
	{
		if (!script.load_property(_rect.x1, "x1"))
			script.load_property(_rect.x1, "x");

		if (!script.load_property(_rect.y1, "y1"))
			script.load_property(_rect.y1, "y");


		script.load_property(_rect.x2, "x2");
		script.load_property(_rect.y2, "y2");


		if (useWH)
		{
			if (script.load_property(_rect.x2, "w"))
				_rect.x2._additive = true;
			if (script.load_property(_rect.y2, "h"))
				_rect.y2._additive = true;
		}


		script.load_property(_pos, "p");

		float margin = 0.0f;
		if (script.load_property(margin, "margin"))
		{
			auto addMargin = [](DestinationProperty &p, float margin)
			{
				if (!p._additive && !p._percent)
					p._value += margin;
			};

			addMargin(_rect.x1, margin);
			addMargin(_rect.y1, margin);
			addMargin(_rect.x2, -margin);
			addMargin(_rect.y2, -margin);
		}
	}

protected:
	Drawer::Destination calculatedDestination()
	{
		auto &oldRect = Drawer::Destination::current().rectangle;
		auto destination = DestinationPropertyRect::destinationClip(_rect);
		if (_pos != -1)
			destination.rectangle.NumLayoutIn(oldRect, _pos);
		return destination;
	}

    DestinationPropertyRect _rect;
	int _pos = -1;
};

class ClipDrawer : public DestinationBaseDrawer
{
public:
	ClipDrawer(LScriptObject& script) : DestinationBaseDrawer(script)
	{

	}

	void DrawBackground() override
	{
		auto destination = calculatedDestination();

		MX::Graphic::ClippingContext clip(destination.rectangle);
		DrawChildren();
	}

protected:
};


class DestinationDrawer : public DestinationBaseDrawer
{
public:
	DestinationDrawer(LScriptObject& script, bool useWH = true) : DestinationBaseDrawer(script, useWH)
	{
		
	}

	void DrawBackground() override
	{
		auto destination = calculatedDestination();

		auto guard = destination.Use();
		DrawChildren();
	}
};

class ShiftDrawer : public DrawerWithChildren
{
public:
    ShiftDrawer(LScriptObject& script) : DrawerWithChildren(script)
    {
        script.load_property(_position, "Position");

        float margin = 0;
        if (script.load_property(margin, "Margin"))
        {
            _position.x = _position.y = -margin;
            _position2.x = _position2.y = margin;
        }

        if (script.load_property(_position, "Offset"))
        {
            _position2 = _position;
        }

    }


    void DrawBackground() override
    {
        auto oldDestination = Drawer::Destination::current();
        
        oldDestination.rectangle.x1 += _position.x;
        oldDestination.rectangle.y1 += _position.y;

        oldDestination.rectangle.x2 += _position2.x;
        oldDestination.rectangle.y2 += _position2.y;
        
        auto guard = oldDestination.Use();
        DrawChildren();
    }

protected:
    glm::vec2 _position;
    glm::vec2 _position2;
};

class TransformDrawer : public DestinationDrawer
{
public:
	TransformDrawer(LScriptObject& script) : DestinationDrawer(script)
	{
		script.load_property(_flipX, "flipX");
		script.load_property(_flipY, "flipY");
	}


	void DrawBackground() override
	{
		bool oldX = Graphic::Image::Settings::flipX;
		bool oldY = Graphic::Image::Settings::flipY;

		Graphic::Image::Settings::flipX = _flipX;
		Graphic::Image::Settings::flipY = _flipY;
		DestinationDrawer::DrawBackground();
		Graphic::Image::Settings::flipX = oldX;
		Graphic::Image::Settings::flipY = oldY;
	}


protected:
	bool _flipX = false;
	bool _flipY = false;
};

class RepeatDrawer : public DestinationDrawer
{
public:
	RepeatDrawer(LScriptObject& script) : DestinationDrawer(script)
	{
		script.load_property(_times, "Times");
		script.load_property(_size, "Size");
	}


	void DrawBackground() override
	{
		int repeat = *_times;
		auto oldDestination = Drawer::Destination::current();
		float size = *_size;
		oldDestination.rectangle.SetWidth(size);

		for (int i = 0; i < repeat; i++)
		{
			auto guard = oldDestination.Use();
			DestinationDrawer::DrawBackground();
			oldDestination.rectangle.Shift(size, 0);
		}
	}


protected:
	Scriptable::Value::pointer _times;
	Scriptable::Value::pointer _size;
};
MXREGISTER_CLASS(L"Repeat", RepeatDrawer);

//tables

class TableDataDrawer : public DestinationDrawer
{
public:
	TableDataDrawer(LScriptObject& script, bool useWH = true) : DestinationDrawer(script, useWH)
	{
		_parentBounds.x2 = -1.0f;
		_parentBounds.y2 = -1.0f;
	}


protected:
	MX::Rectangle _parentBounds;
	DestinationProperty _data;
	bool _vertical = true;
};



class TableDrawer : public TableDataDrawer
{
public:
	TableDrawer(LScriptObject& script, bool useWH = true) : TableDataDrawer(script, useWH)
	{
		GatherChildren();
	}

	void DrawBackground() override
	{
		if (_parentBounds.width() == 0.0f || _parentBounds.height() == 0.0f)
			return;

		auto destination2 = calculatedDestination();
		auto guard2 = destination2.Use();


		auto destination = Drawer::Destination::destinationClip(_parentBounds);
		auto guard = destination.Use();
		_trs.SetBounds();
		_tds.SetBounds();

		for (auto &child : _children)
			child->DrawBackground();
	}

protected:


	void GatherChildren()
	{
		for (auto& child : _children)
		{
			auto t = std::dynamic_pointer_cast<TableDrawer>(child);
			if (!t)
				continue;
			(t->_vertical ? _trs : _tds).AddElement(t);
		}
	}



	struct TableGroup
	{
		TableGroup(bool vertical) : _vertical(vertical)
		{
		}

		void AddElement(const std::shared_ptr<TableDrawer>& elem)
		{
			_elements.push_back(elem);
			auto &d = elem->_data;

			if (!d._loaded)
			{
				//_elementsWithWeight.push_back(elem);
				_elementsWithWeightCount++;
				return;
			}

			if (!d._percent)
			{
				//_elementsWithPixels.push_back(elem);
				_requiredPixels += d.value();
				return;
			}

			_elementsWithPercents.push_back(elem);
		}

		void SetBounds()
		{
			auto &current = Drawer::Destination::current();
			auto maxDimension = _vertical ? current.height() : current.width();
			auto dimensionForWeight = maxDimension;

			if (_elementsWithWeightCount)
			{
				//remove wanted pixels
				dimensionForWeight -= _requiredPixels;

				for (auto &elem : _elementsWithPercents)
					dimensionForWeight -= elem->_data.value(maxDimension);


				dimensionForWeight /= (float)_elementsWithWeightCount;


				if (maxDimension > 0.0f)
				{
					if (dimensionForWeight < 0.0f)
					{
						assert(false);
					}
				}
					
			}
			else
				dimensionForWeight = 0.0f;


			auto offset = 0.0f;
			int count = 0;
			for (auto &elem : _elements)
			{
				count++;
				auto elem_dimension = 0.0f;

				if (!elem->_data._loaded)
					elem_dimension = dimensionForWeight;
				else 
					elem_dimension = elem->_data.value(maxDimension);

				SetElementBound(elem, offset, elem_dimension, count == _elements.size());

				offset += elem_dimension;
			}

		}

		void SetElementBound(const std::shared_ptr<TableDrawer>& elem, float offset, float dimension, bool last)
		{
			auto &current = Drawer::Destination::current();
			MX::Rectangle rect;
			rect.x2 = current.rectangle.width();
			rect.y2 = current.rectangle.height();

			float &d1 = _vertical ? rect.y1 : rect.x1, &d2 = _vertical ? rect.y2 : rect.x2;

			d1 = offset;
			if (!last)
				d2 = offset + dimension;

			elem->_parentBounds = rect;
		}


	protected:
		//std::vector<std::shared_ptr<TableDrawer>> _elementsWithPixels;
		std::vector<std::shared_ptr<TableDrawer>> _elementsWithPercents;
		//std::vector<std::shared_ptr<TableDrawer>> _elementsWithWeight;

		std::vector<std::shared_ptr<TableDrawer>> _elements;

		int _elementsWithWeightCount = 0;
		float _requiredPixels = 0.0f;
		bool _vertical;
	};

	TableGroup _trs = true;
	TableGroup _tds = false;


};


class TRDrawer : public TableDrawer
{
public:
	TRDrawer(LScriptObject& script) : TableDrawer(script, false)
	{
		_vertical = true;
		script.load_property(_data, "h");
	}
};

class TDDrawer : public TableDrawer
{
public:
	TDDrawer(LScriptObject& script) : TableDrawer(script, false)
	{
		_vertical = false;
		script.load_property(_data, "w");
	}
};


void DestinationDrawersInit::Init()
{
	ScriptClassParser::AddCreator(L"Clip", new OutsideScriptClassCreatorContructor<ClipDrawer>());
	ScriptClassParser::AddCreator(L"Destination", new OutsideScriptClassCreatorContructor<DestinationDrawer>());
    ScriptClassParser::AddCreator(L"Shift", new OutsideScriptClassCreatorContructor<ShiftDrawer>());
	ScriptClassParser::AddCreator(L"Transform", new OutsideScriptClassCreatorContructor<TransformDrawer>());

	ScriptClassParser::AddCreator(L"Table", new OutsideScriptClassCreatorContructor<TableDrawer>());

	ScriptClassParser::AddCreator(L"tr", new OutsideScriptClassCreatorContructor<TRDrawer>());
	ScriptClassParser::AddCreator(L"td", new OutsideScriptClassCreatorContructor<TDDrawer>());
}
