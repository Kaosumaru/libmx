#ifndef MXDRAWERMANAGER
#define MXDRAWERMANAGER
#include "MXDrawer.h"

namespace MX
{
namespace Widgets
{

class Widget;


class DrawerManager : public Singleton < DrawerManager >
{
public:
	DrawerManager();

	Drawer::pointer getDrawer(const std::string& objectPath);

	static Drawer::pointer drawer(const std::string& objectPath)
	{
		return DrawerManager::get().getDrawer(objectPath);
	}

	void Deinitialize();
protected:
	void onReload();

	using Map = std::map < std::string, Drawer::pointer > ;
	Map _drawers;
};


}
}

#endif
