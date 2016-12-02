#include "DrawerManager.h"
#include "widgets/Widget.h"
#include "widgets/systems/DragSystem.h"

using namespace MX::Widgets;


class DrawerManagerDrawerProxy : public DrawerProxy
{
public:
	DrawerManagerDrawerProxy(const std::string& objectPath)
	{
		_objectPath = objectPath;
		loadDrawer();
	}

	int version() override 
	{ 
		return _version;
	}

	void loadDrawer()
	{
		_drawer = MX::Script::valueOf(_objectPath).to_object<MX::Widgets::Drawer>();
		_version++;
		assert(_drawer);
	}
protected:
	int _version = 0;
	std::string _objectPath;
};


DrawerManager::DrawerManager()
{
#ifdef _DEBUG
	Script::onParsed.connect([&]() { onReload(); });
#endif
}

void DrawerManager::Deinitialize()
{
	_drawers.clear();
}


void DrawerManager::onReload()
{
	for (auto& pair : _drawers)
	{
		auto drawer = std::static_pointer_cast<DrawerManagerDrawerProxy>(pair.second);
		drawer->loadDrawer();
	}

}

Drawer::pointer DrawerManager::getDrawer(const std::string& objectPath)
{
	auto &drawer = _drawers[objectPath];
	if (drawer)
		return drawer;

#ifdef _DEBUG
	drawer = std::make_shared<DrawerManagerDrawerProxy>(objectPath);
#else
	drawer = Script::valueOf(objectPath).to_object<MX::Widgets::Drawer>();
#endif

	assert(drawer);

	return drawer;
}