#include "TooltipSystem.h"
#include "application/Window.h"
#include "game/resources/Resources.h"
#include <iostream>



using namespace MX;
using namespace MX::Widgets;



void TooltipEntry::Show()
{
	if (_system)
		return;
	_system = &(TooltipSystem::current());
	if (_system)
	{
		onShow();
		_system->AddEntry(this);
	}
		
}

void TooltipEntry::Hide()
{
	if (!_system)
		return;
	_system->RemoveEntry(this);
	_system = nullptr;
	onHide();
}




TooltipSystem::TooltipSystem()
{
	
}

void TooltipSystem::Draw()
{
	for (auto &e : _entries)
		e->onDraw();
}

void TooltipSystem::AddEntry(TooltipEntry* entry)
{
	_entries.push_back(entry);
}
void TooltipSystem::RemoveEntry(TooltipEntry* entry)
{
	auto it = std::find(_entries.begin(), _entries.end(), entry);
	if (it == _entries.end())
	{
		assert(false);
		return;
	}
	_entries.erase(it);
}
