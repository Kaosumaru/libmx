#include "ProxyDrawers.h"
#include "Drawers.h"
#include "graphic/Blender.h"
#include "graphic/images/TextureRental.h"
#include "graphic/renderer/MVP.h"
#include "script/ScriptClassParser.h"
#include "widgets/Widget.h"

using namespace MX;

class SurfaceProxyDrawer : public MX::Widgets::DrawerProxy
{
public:
    SurfaceProxyDrawer(LScriptObject& script)
        : MX::Widgets::DrawerProxy(script)
    {
        script.load_property(_inhibitNormalDrawing, "InhibitNormalDrawing");
        script.load_property(_clearColor, "ClearColor");

        script.load_property(_passDrawers, "PassDrawers");

        if (!script.load_property(_postDrawers, "PostDrawers"))
        {
            Drawer::pointer postDrawer;
            if (!script.load_property(postDrawer, "PostDrawer"))
                postDrawer = std::make_shared<MX::Widgets::ImageDrawer>();
            _postDrawers.push_back(postDrawer);
        }
    }

    void DrawBackground() override
    {
        auto& w = MX::Widgets::Widget::current();
        if (!Context<Drawer::ContextData>::isCurrent())
            return;
        Context<Drawer::ContextData>::current().drawChildren = !_inhibitNormalDrawing;

        const auto& dimensions = w.dimensions();

        auto& rental = MX::Graphic::TextureRental::get();
        auto rentedSurface = rental.rentTexture(dimensions, true);
        auto surface1 = rentedSurface.surface();

        auto rentedSurface2 = rental.rentTexture(dimensions, true);
        auto surface2 = rentedSurface2.surface();

        DrawWidgetOnSurface(surface1);
        DoSteps(surface1, surface2);

#if 0
		static bool test = false;
		if (test)
		{
			_surface->save("d:/shader.png");
		}
#endif

        auto guard = Context<Graphic::Image>::Lock(surface1);
        auto guard2 = Graphic::Blender::defaultPremultiplied().Use();

        for (auto& drawer : _postDrawers)
            drawer->DrawBackground();
    }

    void DrawForeground() override
    {
        auto guard2 = Graphic::Blender::defaultPremultiplied().Use();

        for (auto& drawer : _postDrawers)
            drawer->DrawForeground();
    }

protected:
    void DrawWidgetOnSurface(std::shared_ptr<Graphic::TextureImage>& surface1)
    {
        auto& w = MX::Widgets::Widget::current();

        {
            Graphic::TargetContext context(*surface1);
            surface1->Clear(_clearColor);

            static Graphic::Blender blender(GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            auto guard = blender.Use();
            MVP::Push();
            MVP::translate(glm::vec2 { -(int)w.geometry.position.x, -(int)w.geometry.position.y }); //#WIDGETFLOAT HERE, BUG if y has fraction part

            _drawer->DrawBackground();
            widgetDrawChildren();
            _drawer->DrawForeground();

            MVP::Pop();
        }
    }

    void DoSteps(std::shared_ptr<Graphic::TextureImage>& surface1, std::shared_ptr<Graphic::TextureImage>& surface2)
    {
        auto& w = MX::Widgets::Widget::current();
        const auto& dimensions = w.dimensions();
        if (_passDrawers.empty())
            return;

        auto dest_rect = Drawer::Destination(Rectangle::fromWH(0.0f, 0.0f, dimensions.x, dimensions.y));
        auto guard_dest = Context<Drawer::Destination>::Lock(dest_rect);

        for (auto& drawer : _passDrawers)
        {
            auto iguard = Context<Graphic::Image>::Lock(surface1); //draw from this...
            Graphic::TargetContext context(*surface2); //to this

            auto blender = MX::Graphic::Blender::defaultCopy().Use();
            drawer->DrawBackground();

            std::swap(surface1, surface2);
        }
    }

    bool _inhibitNormalDrawing = true;
    MX::Color _clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };

    std::vector<Drawer::pointer> _passDrawers;
    std::vector<Drawer::pointer> _postDrawers;
};

void MX::Widgets::ProxyDrawersInit::Init()
{
    ScriptClassParser::AddCreator(L"Drawer.Proxy.Surface", new OutsideScriptClassCreatorContructor<SurfaceProxyDrawer>());
}