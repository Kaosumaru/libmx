#ifndef MXSCRIPTLAYOUTERS
#define MXSCRIPTLAYOUTERS
#include "widgets/Widget.h"
#include "WidgetFactory.h"

namespace MX
{
namespace Widgets
{


	class ScriptLayouterWidget;

	class ScriptLayouter : public ScriptObject, public shared_ptr_init<ScriptLayouter>
	{
	public:
		using NamedWidgetsMap = std::multimap < std::string, std::shared_ptr<Widget> >;

		ScriptLayouter() {}
		ScriptLayouter(ScriptLayouter& other) {}
		ScriptLayouter(LScriptObject& script){}
		virtual ~ScriptLayouter(){};

		virtual void OnLinkedToWidget() {};

		virtual void LayoutWidget(const std::string& name, const std::shared_ptr<Widget> &widget) {}

		virtual void RelayoutAllWidgets(const NamedWidgetsMap& named_widgets)
		{
			for (auto& pair : named_widgets)
				LayoutWidget(pair.first, pair.second);
		}

		virtual void clipSize(float &width, float &height) {}
		virtual void DrawBackground() {}

		virtual int version() { return 0; }
	protected:

		void ChangeInnerDimension(const glm::vec2& size);
	};


	class ScriptLayouterBase : public ScriptLayouter
	{
	public:
		ScriptLayouterBase(LScriptObject& script);
		void OnLinkedToWidget() override;
		void LayoutWidget(const std::string& name, const std::shared_ptr<Widget> &widget) override;
	protected:
		virtual void FindDrawerForWidget(const std::string& name, const std::shared_ptr<Widget> &widget);

		void FindDrawersForAllWidgets(const NamedWidgetsMap& named_widgets)
		{
			for (auto &pair : named_widgets)
				FindDrawerForWidget(pair.first, pair.second);
		}

		std::shared_ptr<Drawer> _drawer;
		std::vector<WidgetCreationData> _customWidgets;
		std::map<std::string, std::shared_ptr<Drawer>> _drawers;
		std::map<std::string, std::string> _layouters;
	};



	class ScriptLayouterManager : public Singleton < ScriptLayouterManager >
	{
	public:
		ScriptLayouterManager();

		ScriptLayouter::pointer getLayouter(const std::string& objectPath);

		static ScriptLayouter::pointer layouter(const std::string& objectPath)
		{
			return ScriptLayouterManager::get().getLayouter(objectPath);
		}

		void Deinitialize();
	protected:
		void onReload();

		using Map = std::map < std::string, ScriptLayouter::pointer >;
		Map _layouters;
	};


	class ScriptLayouterWidget : public Widget
	{
	public:
		using NamedWidgetsMap = std::multimap < std::string, std::shared_ptr<Widget> > ;

		ScriptLayouterWidget();

		void SetDefaultWidgetName(const std::string& name);
		void AddWidget(const std::shared_ptr<Widget> &widget);

		virtual NamedWidgetsMap::iterator AddNamedWidget(const std::string& name, const std::shared_ptr<Widget> &widget);
		void AddLayouterWidget(const std::string& name, const std::shared_ptr<Widget> &widget);

		void SetLayouter(const std::string& objectPath);
		void SetLayouter(const ScriptLayouter::pointer &layouter);
		const ScriptLayouter::pointer &layouter();


		void InvalidateLayout() { _layoutDirty = true; }

		void clipSize(float &width, float &height) override;

		const auto& namedWidgets() { return _namedWidgets; }

		void Clear() override;
	protected:
		void onChildRemoved(Widget& child) override;
		void onChildSizeChanged() override;
		void RelayoutIfNeeded();

		void Run() override;
		void DrawBackground() override;

		void RemoveLayouterWidgets();

		void OnAreaInsideChanged() override;

		void relayoutNamedWidgets();

#ifdef _DEBUG
		int  _layouterVersion = 0;
#endif

		std::string             _defaultWidgetName;
		bool _layoutDirty = true;
		ScriptLayouter::pointer _layouter;
		NamedWidgetsMap _namedWidgets;

		std::vector<NamedWidgetsMap::iterator> _layouterWidgets;
	};




	class ScriptLayouterInit
	{
	public:
		static void Init();
	};
}
}

#endif
