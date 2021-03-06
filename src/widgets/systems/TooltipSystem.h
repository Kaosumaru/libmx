#pragma once
#include "utils/Singleton.h"
#include "utils/Utils.h"
#include <map>
#include <set>
#include <vector>

namespace MX
{
namespace Widgets
{

    class TooltipSystem;

    class TooltipEntry
    {
    public:
        friend class TooltipSystem;

        virtual ~TooltipEntry()
        {
            Hide();
        }

    protected:
        virtual void onDraw() {};
        virtual void onShow() {};
        virtual void onHide() {};

        void Show();
        void Hide();

        TooltipSystem* _system = nullptr;
    };

    class TooltipSystem : public shared_ptr_init<TooltipSystem>
    {
    public:
        friend class TooltipEntry;

        TooltipSystem();
        void Draw();

        static TooltipSystem& current()
        {
            return ScopeSingleton<TooltipSystem>::current();
        }

    protected:
        void AddEntry(TooltipEntry* entry);
        void RemoveEntry(TooltipEntry* entry);

        std::vector<TooltipEntry*> _entries;
    };

}
}
