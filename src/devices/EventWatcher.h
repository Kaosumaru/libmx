#pragma once
#include "SDL_events.h"

class SDLEventWatcher
{
public:
    SDLEventWatcher()
    {
        SDL_AddEventWatch(EventFilter, this);
    }

    ~SDLEventWatcher()
    {
        SDL_DelEventWatch(EventFilter, this);
    }

    static int EventFilter(void* userdata, SDL_Event* event)
    {
        ((SDLEventWatcher*)userdata)->OnEvent(*event);
        return 0;
    }

protected:
    virtual void OnEvent(SDL_Event& event) = 0;
};