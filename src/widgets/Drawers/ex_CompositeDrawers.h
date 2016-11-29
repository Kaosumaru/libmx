#ifndef MXCOMPOSITEDRAWERS
#define MXCOMPOSITEDRAWERS
#include "Utils/MXUtils.h"
#include "MXDrawer.h"
#include "Graphic/Images/MXImage.h"
#include "Graphic/Images/MXSlice9Image.h"
#include "MXDrawerUtils.h"

namespace MX
{

namespace Widgets
{


	class CompositeDrawer : public Widgets::Drawer
	{
	public:
		using SizeType = std::pair<FloatOrExpression, FloatOrExpression>;

		CompositeDrawer(LScriptObject& script);

		std::unique_ptr<ShapePolicy> defaultShapePolicy() override;
		boost::optional<MX::Margins> defaultMargins() override { return _optionalMargins; }
		
		boost::optional<MX::Vector2> defaultSize() override 
		{ 
			if (!_defaultSize)
				return boost::none;
			return MX::Vector2{ _defaultSize->first, _defaultSize->second };
		}

		void clipSize(float &width, float &height) override;

		void OnAssignedToWidget() override;
	protected:
		std::map<std::string, float> _properties;

		
		SizeType _minSize;
		SizeType _maxSize = { -1.0f, -1.0f };


		std::shared_ptr<ShapePolicy> _shapePolicy;
		boost::optional<MX::Margins> _optionalMargins;
		boost::optional<SizeType> _defaultSize;
	};

	class CompositeLayouterDrawer : public CompositeDrawer
	{
	public:

		struct LayouterEntry
		{
			LayouterEntry(const std::shared_ptr<Widgets::Drawer> &drawer, const MX::Rectangle &rectangle) :drawer(drawer), rectangle(rectangle) {}

			std::shared_ptr<Widgets::Drawer> drawer;
			MX::Rectangle rectangle;

			void DrawBackground()
			{
				if (!drawer)
					return;
				auto destination = Drawer::Destination::destinationClip(rectangle);
				auto guard = destination.Use();
				drawer->DrawBackground();
			}
		};

		typedef std::vector<LayouterEntry> List;

		CompositeLayouterDrawer(LScriptObject& script);

		void AddChild(const std::shared_ptr<Widgets::Drawer> &drawer, const MX::Rectangle &rectangle = MX::Rectangle(0.0f, 0.0f, -1.0f, -1.0f));
		void DrawBackground() override;



	protected:

		List _children;
	};


	class CompositeDrawersInit
	{
	public:
		static void Init();
	};

}
}

#endif
