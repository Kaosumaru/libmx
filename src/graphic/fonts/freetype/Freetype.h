#pragma once
#include "glm/vec4.hpp"
#include "utils/Singleton.h"
#include "utils/Utf8.h"
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace MX
{
namespace Graphic
{

    class Freetype : public DeinitSingleton<Freetype>
    {
    public:
        Freetype()
        {
            FT_Init_FreeType(&_library);
            //error
        }

        ~Freetype();

        static auto& library()
        {
            return Freetype::get()._library;
        }

    protected:
        FT_Library _library;
    };

    class Glyph
    {
    public:
        friend class Face;
        Glyph() { }
        Glyph(const Glyph& other) = delete;

        auto& glyph() { return _glyph; }

        template <typename T>
        auto iterate_bitmap(T&& t)
        {
            auto g = (FT_BitmapGlyph)_glyph;
            int ox = g->left;
            int oy = -g->top;

            FT_Int p, q;
            FT_Int x_max = g->bitmap.width;
            FT_Int y_max = g->bitmap.rows;

            auto pointer = g->bitmap.buffer;
            for (q = oy; q < y_max + oy; q++)
            {
                for (p = ox; p < x_max + ox; p++)
                {
                    t(p, q, *pointer);
                    pointer++;
                }
            }

            return g->root.advance.x;
        }

        ~Glyph()
        {
            if (_glyph)
                FT_Done_Glyph(_glyph);
        }

        int height()
        {
            auto g = (FT_BitmapGlyph)_glyph;
            return g->bitmap.rows;
        }

    protected:
        FT_UInt _index = 0;
        FT_Glyph _glyph = nullptr;
    };

    class Face
    {
    public:
        Face(const std::string& path, unsigned size)
        {
            auto error = FT_New_Face(Freetype::library(), path.c_str(), 0, &_face);
            _hasKerning = FT_HAS_KERNING(_face);
            SetCharSize((int)size << 6);
            _size = size;
            //WIPerror
        }

        static std::shared_ptr<Face> Create(const std::string& path, unsigned size);

        ~Face()
        {
            Clear();
        }

        void Clear()
        {
            _charToGlyph.clear();
            if (_face)
                FT_Done_Face(_face);
            _face = nullptr;
        }

        Glyph& LoadCharCached(FT_ULong c)
        {
            auto& cached_glyph = _charToGlyph[c];
            if (!cached_glyph.glyph())
            {
                LoadGlyph(c, cached_glyph);
            }

            return cached_glyph;
        }

        template <typename Text, typename T>
        int draw_text(const Text* text, FT_Vector pen, const T&& t)
        {
            FT_UInt previous = 0;
            auto current = MX::utf8::next_character(text);
            while (current != 0)
            {
                auto& glyph = LoadCharCached(current);

                if (_hasKerning && previous && glyph._index)
                {
                    FT_Vector delta;
                    FT_Get_Kerning(_face, previous, glyph._index, FT_KERNING_DEFAULT, &delta);
                    pen.x += delta.x >> 6;
                }

                auto advance_x = glyph.iterate_bitmap([&](int x, int y, uint8_t p) {
                    t(x + pen.x, y + pen.y, p);
                });

                pen.x += advance_x >> 16;
                previous = glyph._index;
                current = MX::utf8::next_character(text);
            }
            return pen.x;
        }

        auto face() { return _face; }

        int ascender() const { return _face->ascender >> 6; }
        int descender() const { return _face->descender >> 6; }
        int height() const { return _face->height >> 6; }
        int x_height()
        {
            auto& x = LoadCharCached('x');
            return x.height();
        }

        int X_height()
        {
            auto& x = LoadCharCached('X');
            return x.height();
        }

        int size() { return _size; }

    protected:
        void SetCharSize(FT_F26Dot6 width, FT_F26Dot6 height = 0, FT_UInt hres = 100, FT_UInt vres = 0)
        {
            auto error = FT_Set_Char_Size(_face, width, height, hres, vres);
            //WIPerror
        }

        FT_GlyphSlot glyph_slot()
        {
            return _face->glyph;
        }

        FT_ULong GetCharIndex(FT_ULong c)
        {
            return FT_Get_Char_Index(_face, c);
        }

        bool LoadGlyph(FT_ULong c, Glyph& glyph)
        {
            auto glyph_index = FT_Get_Char_Index(_face, c);

            int error = FT_Load_Glyph(_face, glyph_index, FT_LOAD_RENDER);
            if (error != 0)
                return false;

            auto slot = glyph_slot();
            error = FT_Get_Glyph(slot, &glyph.glyph());
            if (error != 0)
                return false;
            glyph._index = glyph_index;
            return true;
        }

        bool _hasKerning;
        std::map<FT_ULong, Glyph> _charToGlyph;
        FT_Face _face;
        int _size = 0;
    };

    class TextureImage;

    struct FreetypeUtils
    {
        static std::shared_ptr<Graphic::TextureImage> drawLine(const std::shared_ptr<Face>& face, const std::string& text);
        static std::shared_ptr<Graphic::TextureImage> drawLine(const std::shared_ptr<Face>& face, const std::wstring& text);

        static int measureLine(const std::shared_ptr<Face>& face, const std::string& text);
        static int measureLine(const std::shared_ptr<Face>& face, const std::wstring& text);
    };
}
}
