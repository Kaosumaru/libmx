#pragma once

#include "graphic/Color.h"
#include "graphic/opengl/Texture.h"
#include "utils/Singleton.h"
#include "utils/Utils.h"
#include "utils/Vector2.h"
#include "utils/shapes/Rectangle.h"

namespace MX
{
namespace Graphic
{

    class TextureRenderer : public ScopeSingletonFunction<TextureRenderer>, public shared_ptr_init<TextureRenderer>
    {
    public:
        friend class ScopeSingletonFunction<TextureRenderer>;

        virtual ~TextureRenderer() { }

        virtual void Flush() {};
        virtual void Draw(const gl::Texture& tex, const Rectangle& srcArea, const glm::vec2& pos, const glm::vec2& relativeCenter, const glm::vec2& size, const MX::Color& color, float angle) = 0;

    protected:
        void StartBatch()
        {
            if (_count++ == 0)
                OnStarted();
        }

        void EndBatch()
        {
            if (--_count == 0)
                OnEnded();
        }

        static void CurrentWasSet(TextureRenderer* target, TextureRenderer* old_target)
        {
            if (target == old_target)
                return;
            if (old_target)
            {
                old_target->EndBatch();
                old_target->OnNoLongerCurrent();
            }

            if (target)
            {
                target->OnSetAsCurrent();
                target->StartBatch();
            }
        }

        virtual void OnSetAsCurrent() { }
        virtual void OnNoLongerCurrent() { }

        virtual void OnStarted() { }
        virtual void OnEnded() { }

        int _count = 0;
    };

}
}
