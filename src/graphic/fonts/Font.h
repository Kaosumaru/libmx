#pragma once
#include "graphic/images/TextureImage.h"
#include "utils/Utils.h"
#include <memory>
#include <string>

namespace MX
{
namespace Graphic
{
    class Face;

    class Font : public shared_ptr_init<Font>, virtual public disable_copy_constructors
    {
    public:
        Font(const std::string& path, float size, const std::string& face_name = "");
        virtual ~Font();

        static pointer Create(const std::string& path, float size, const std::string& face_name = "");
        static pointer CreateDefault();

        std::shared_ptr<TextureImage> DrawTextOnBitmap(const std::string& str);
        std::shared_ptr<TextureImage> DrawTextOnBitmap(const std::wstring& str);

        bool empty();

        const auto& size() { return _size; }
        const auto& path() { return _path; }
        const auto& face_name() { return _face_name; }
        const auto& face() { return _ftFace; }
        const auto& faceBold() { return _ftFaceBold; }
        float ascender();
        float X_height();

        void ReloadFace(const std::string& path);
        void ReloadFaceBold(const std::string& path);

    protected:
        std::string _face_name;
        std::string _path;
        float _size = 16.0f;
        std::shared_ptr<Face> _ftFace;
        std::shared_ptr<Face> _ftFaceBold;
    };
}
}
