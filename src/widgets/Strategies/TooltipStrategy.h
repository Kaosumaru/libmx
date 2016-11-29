#ifndef MXTOOLTIPSTRATEGY
#define MXTOOLTIPSTRATEGY
#include "MXStrategy.h"
#include "Widgets/Systems/MXTooltipSystem.h"
#include "Utils/MXVector2.h"
#include "Widgets/MXWidget.h"

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


	Vector2      _position;
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
