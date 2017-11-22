#include "TestManager.h"
#include "Game/Resources/Paths.h"
#include "Game/Resources/Resources.h"
#include "script/Script.h"
#include "script/ScriptObject.h"
#include "application/Window.h"

#include "graphic/renderer/DefaultRenderers.h"
#include "html/HTMLRendererFreetype.h"

#include "devices/Mouse.h"
#include "devices/Keyboard.h"
#include "application/Window.h"
#include "game/ScriptInitializer.h"
#include "graphic/fonts/Font.h"
#include "script/ScriptClassParser.h"
#include "utils/Random.h"
#include <iostream>


using namespace MX;


namespace
{
    float maxX = 800.0f, maxY = 600.0f;
    float minX = 0.0f, minY = 0.0f;
    float gravity = 0.75f;

    int amount = 500;
    int startBunnyCount = 10;
    int isAdding = false;
    int bcount = 0;
}

class Bunny : public ImageSpriteActor
{
public:
    Bunny()
    {
        SetImage( GetBunnyImage() );
        geometry.position.x = 0.0f;
        geometry.position.y = 0.0f;

        _speed = { Random::randomRange( std::make_pair( 0.0f, 10.0f ) ), Random::randomRange( std::make_pair( -5.0f, 5.0f ) ) };
    }

    const std::shared_ptr<Graphic::TextureImage>& GetBunnyImage()
    {
        static std::shared_ptr<Graphic::TextureImage> image;
        if (!image) image = Resources::get().loadImage( "bunny.png" );
        return image;
    }


    void Run() override
    {
        geometry.position += _speed;
        _speed.y += gravity;

        if( geometry.position.x > maxX )
        {
            _speed.x *= -1;
            geometry.position.x = maxX;
        }
        else if( geometry.position.x < minX )
        {
            _speed.x *= -1;
            geometry.position.x = minX;
        }

        if( geometry.position.y > maxY )
        {
            _speed.y *= -0.85;
            geometry.position.y = maxY;
            //bunny.spin = ( Math.random() - 0.5 ) * 0.2
            if( Random::chance(0.5f) )
            {
                _speed.y -= Random::randomRange( std::make_pair( 0.0f, 6.0f ) );
            }
        }
        else if( geometry.position.y < minY )
        {
            _speed.y = 0;
            geometry.position.y = minY;
        }
    }

protected:
    glm::vec2 _speed;
};

void AddBunny( TestManager* m )
{
    auto bunny = std::make_shared<Bunny>();
    m->AddActor( bunny );
    bcount++;
}

void AddBunnies( TestManager* m, int x)
{
    for( int i = 0; i < x; i++ )
    {
        AddBunny( m );
    }
}

TestManager::TestManager() : DisplaySceneTimer(MX::Window::current().size())
{
    //_cheats = CreateCheats();
    maxX = MX::Window::current().size().x;
    maxY = MX::Window::current().size().y;


	_visibility = -1;

	MX::Window::current().keyboard()->on_specific_key_down[SDL_SCANCODE_SPACE].connect(std::bind(&TestManager::reloadScripts, this), this);
	MX::Window::current().keyboard()->on_specific_key_down[SDL_SCANCODE_R].connect(std::bind(&TestManager::clearReloadScripts, this), this);

    MX::Window::current().mouse()->on_specific_button_down[1].static_connect( []( auto& x ) { isAdding = true;  } );
    MX::Window::current().mouse()->on_specific_button_up[1].static_connect( []( auto& x ) { isAdding = false;  } );
    MX::Window::current().mouse()->on_specific_button_up[3].static_connect( []( auto& x ) { std::cout << "Bunnies " << bcount << "\n";  } );


	reloadScripts();

    Context<BaseGraphicScene>::SetCurrent(*this);

    AddBunnies( this, startBunnyCount );
}

void TestManager::AddSomething()
{
#if 0
	std::shared_ptr<SpriteActor> actor = Script::valueOf("Game.Resources.Explosion.ExplosionTest").to_object<SpriteActor>();
	if (!actor)
		return;
	auto clone = actor->clone();
	clone->geometry.position = point;
#endif
}


void TestManager::clearReloadScripts()
{
#ifdef WIP
	MX::Resources::get().Clear();
#endif
	reloadScripts();
}

void TestManager::reloadScripts()
{


}


void TestManager::Run()
{
    if( isAdding )
    {
        AddBunnies( this, amount );
    }
    MX::DisplaySceneTimer::Run();
}

void TestManager::Draw(float x, float y)
{
    MX::DisplaySceneTimer::Draw(x, y);
}



