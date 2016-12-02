#ifndef MXTOOLTIPSTRATEGY
#define MXTOOLTIPSTRATEGY
#include "Strategy.h"
#include "widgets/systems/TooltipSystem.h"
#include "utils/Vector2.h"
#include "widgets/Widget.h"

namespace MX
{
namespace Widgets
{


namespace Strategy
{



class Tooltip : public Interactive, public TooltipEntry
{
public:
	Tooltip(const std::string& drawerPath);
	Tooltip(const std::wstring& text, const std::string& drawerPath);

	bool runnable() override { return true; }
	bool Run() override;
protected:
	void onDraw() override;
	void onShow() override;
	void onHide() override;

	void OnInit() override;
	void onHoverIn();
	void onHoverOut();


	glm::vec2      _position;
	std::string  _drawerPath;
	std::wstring _tooltipText;

	WidgetPtr    _tooltipWidget;

	bool         _wasHover = false;
	float        _showTime = 0.5f;
	std::shared_ptr<int> _conn;
};



}

}

}




#endif
