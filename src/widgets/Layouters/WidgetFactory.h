#ifndef MXWIDGETFACTORY
#define MXWIDGETFACTORY
#include "Widgets/MXWidget.h"
#include "Script/MXPropertyLoaders.h"

namespace MX
{
namespace Widgets
{

	class WidgetFactory : public ScriptObject
	{
	public:
		WidgetFactory(LScriptObject& script);
		virtual ~WidgetFactory() {}

		WidgetPtr Create();

	protected:
		virtual WidgetPtr WidgetCreate() { return MX::make_shared<MX::Widgets::Widget>(); }

		std::shared_ptr<Drawer> _drawer;
	};


	struct WidgetCreationData
	{
		std::string name;
		std::shared_ptr<WidgetFactory> factory;
	};





	class WidgetFactoryInit
	{
	public:
		static void Init();
	};
}


template<>
struct PropertyLoader<Widgets::WidgetCreationData>
{
	using type = PropertyLoader_Standard;
	static bool load(Widgets::WidgetCreationData& out, const Scriptable::Value::pointer& obj)
	{
		if (obj->size() > 1)
		{
			
			PropertyLoader< std::string >::load(out.name, obj->array()[0]);
			PropertyLoader< std::shared_ptr<Widgets::WidgetFactory> >::load(out.factory, obj->array()[1]);
			
			return true;
		}
		else
		{
			out.name = obj->atext();
			return true;
		}

		return false;
	}
};

}

#endif
