#ifndef MXCOMPOSITEDRAWERS
#define MXCOMPOSITEDRAWERS
#include "Utils/Utils.h"
#include "Drawer.h"
#include "Graphic/Images/Image.h"
#include "Graphic/Images/Slice9Image.h"
#include "DrawerUtils.h"

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
		stx::optional<MX::Margins> defaultMargins() override { return _optionalMargins; }
		
		stx::optional<glm::vec2> defaultSize() override 
		{ 
			if (!_defaultSize)
				return stx::optional<glm::vec2>{};
			return glm::vec2{ _defaultSize->first, _defaultSize->second };
		}

		void clipSize(float &width, float &height) override;

		void OnAssignedToWidget() override;
	protected:
		std::map<std::string, float> _properties;

		
		SizeType _minSize;
		SizeType _maxSize = { -1.0f, -1.0f };


		std::shared_ptr<ShapePolicy> _shapePolicy;
		stx::optional<MX::Margins> _optionalMargins;
		stx::optional<SizeType> _defaultSize;
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
