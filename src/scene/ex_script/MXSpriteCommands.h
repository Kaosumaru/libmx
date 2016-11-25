#ifndef MXSPRITECOMMANDS
#define MXSPRITECOMMANDS
#include "MXCommand.h"
#include "Utils/MXUtils.h"
#include <memory>
#include <boost/signals2/signal.hpp>
#include "Scene/Sprites/MXScriptableSpriteActor.h"

namespace bs2 = boost::signals2;

namespace MX
{
	std::shared_ptr<Command> unlink();
	std::shared_ptr<Command> unlink_if_empty();
	std::shared_ptr<Command> change_color(const Color& color);
	std::shared_ptr<Command> teleport_to(const Vector2 &vec);
	std::shared_ptr<Command> teleport_to(float x, float y);
	std::shared_ptr<Command> lerp_color(const Color &color1, const Color &color2, float seconds);
    std::shared_ptr<Command> lerp_color(const Color &color, float seconds);
	std::shared_ptr<Command> blink_color(const Color &color, float seconds);
	std::shared_ptr<Command> move_to(float x, float y, float seconds);
	std::shared_ptr<Command> warp_scale(float x, float y, float seconds);
    std::shared_ptr<Command> move_in_direction(const Vector2 &vec, float speed);
	std::shared_ptr<Command> nail_to(const SpriteActorPtr& original, const SpriteActorPtr& ptr);



	struct glue_to_options
	{
		glue_to_options() {}
		Vector2 offset;
		bool shouldRotate = true;
		bool absolute_position = false;
		bool surviveParent = false;
		bool scale_offset = false;
		bool inherit_scale = false;
	};
	std::shared_ptr<Command> glue_to(const SpriteActorPtr& ptr, const glue_to_options& options = glue_to_options{});


	class ScriptableCommandsInit
	{
	public:
		static void Init();
	};
}


#endif
