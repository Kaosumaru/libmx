#pragma once
#include <memory>
#include <vector>
#include <map>
#include <fstream>
#include "graphic/images/TextureImage.h"
#include "utils/Utf8.h"
#include "script/PropertyLoaders.h"

namespace MX::Graphic
{
	namespace bmfont
	{
		struct Header
		{
			uint8_t type;
			uint32_t length;

			void Read(std::ifstream& file);
		};

		struct Info
		{
			static constexpr uint8_t type = 1;
			int16_t fontSize; //	2 	int 	0
			uint8_t bitField; //	1 	bits 	2 	bit 0: smooth, bit 1 : unicode, bit 2 : italic, bit 3 : bold, bit 4 : fixedHeigth, bits 5 - 7 : reserved
			uint8_t charSet; // 	1 	uint 	3
			uint16_t stretchH; // 	2 	uint 	4
			uint8_t aa; // 			1 	uint 	6
			uint8_t paddingUp; // 	1 	uint 	7
			uint8_t paddingRight; //	1 	uint 	8
			uint8_t paddingDown; // 	1 	uint 	9
			uint8_t paddingLeft; // 	1 	uint 	10
			uint8_t spacingHoriz; //	1 	uint 	11
			uint8_t spacingVert; // 	1 	uint 	12
			uint8_t outline; // 	1 	uint 	13	added with version 2
			std::string fontName; // 	n + 1	string 	14	null terminated string with length n
								  //fontName 	n + 1	string 	14	null terminated string with length n

			bool Read(std::ifstream& file);
		};

		struct Common
		{
			static constexpr uint8_t type = 2;
			static constexpr int static_size = 15;
			uint16_t lineHeight;
			uint16_t base;
			uint16_t scaleW;
			uint16_t scaleH;
			uint16_t pages;
			uint8_t bitField;
			uint8_t alphaChnl;
			uint8_t redChnl;
			uint8_t greenChnl;
			uint8_t blueChnl;

			bool Read(std::ifstream& file);
		};

		struct Pages
		{
			static constexpr uint8_t type = 3;

			char* page(unsigned i)
			{
				return data.get() + i * page_len;
			}

			bool Read(std::ifstream& file);

			int pages = 0;
		protected:
			int page_len;
			std::unique_ptr<char[]> data;
		};

		struct Chars
		{
			static constexpr uint8_t type = 4;

			struct Char
			{
				uint32_t id;
				uint16_t x;
				uint16_t y;
				uint16_t width;
				uint16_t height;
				int16_t xoffset;
				int16_t yoffset;
				int16_t xadvance;
				uint8_t page;
				uint8_t chnl;

				static constexpr int size = 20;
			};

			Char* character_for_index(unsigned i)
			{
				auto ptr = data.get() + i * Char::size;
				return (Char*)ptr;
			}

			Char* character(uint32_t id)
			{
				auto it = id_chars.find(id);
				if (it == id_chars.end()) return nullptr;
				return it->second;
			}

			const auto& characters()
			{
				return id_chars;
			}

			bool Read(std::ifstream& file);

		protected:
			std::map<uint32_t, Char*> id_chars;
			std::unique_ptr<char[]> data;
		};

		struct Kernings
		{
			static constexpr uint8_t type = 5;
			bool Read(std::ifstream& file);

			int16_t kerning(uint32_t a, uint32_t b)
			{
				auto it = kernings.find(std::make_pair(a, b));
				if (it == kernings.end()) return 0;
				return it->second;
			}
		protected:
			std::map<std::pair<uint32_t, uint32_t>, int16_t> kernings;
		};
	}

	class RenderQueue
	{
	public:
		struct Item
		{
			std::shared_ptr<MX::Graphic::TextureImage> glyph;
			glm::vec2 pos;
			float scale = 1.0f;
			void Render(const glm::vec2& p, const Color &color) const
			{
				glyph->DrawCentered({}, p + pos, {scale, scale}, 0.0f, color);
			}

			glm::vec2 bounds() const
			{
				return pos + glm::vec2{glyph->dimensions()} *scale;
			}
		};

		using Queue = std::vector<Item>;

		RenderQueue()
		{

		}

		RenderQueue(const RenderQueue& other) : _queue(other._queue), _bounds(other._bounds)
		{
		}

		RenderQueue(RenderQueue&& other) : _queue(std::move(other._queue)), _bounds(other._bounds)
		{
		}

		RenderQueue& operator = (RenderQueue&& other)
		{
			_queue = std::move(other._queue);
			_bounds = other._bounds;
			return *this;
		}


		void AddItem(const Item& item)
		{
			_queue.push_back(item);
			_bounds = glm::max(_bounds, item.bounds());
		}

		void Render(const glm::vec2& pos = {}, const Color &color = Color::white()) const
		{
			for (auto& item : _queue)
			{
				item.Render(pos, color);
			}
		}

		void Clear()
		{
			_bounds = {};
			_queue.clear();
		}

		void SetBounds(const glm::vec2& bounds) { _bounds = bounds;  }
		const auto& bounds() const { return _bounds; }
		const auto& items() const { return _queue; }
	protected:
		glm::vec2 _bounds;
		Queue _queue;
	};

	class BitmapFont
	{
	public:
		using pointer = std::shared_ptr<BitmapFont>;
		class Character
		{
		public:
			friend class BitmapFont;
			bmfont::Chars::Char* info() { return _info; }
			const auto& image() { return _image; }
		protected:
			bmfont::Chars::Char* _info;
			std::shared_ptr<MX::Graphic::TextureImage> _image;
		};

		BitmapFont()
		{

		}

		static pointer Create(const std::string& path)
		{
			auto ptr = std::make_shared<BitmapFont>();
			if (!ptr->load(path)) return nullptr;
			return ptr;
		}

		bool load(const std::string& path);

		Character* character(uint32_t id)
		{
			auto it = _characters.find(id);
			if (it == _characters.end()) return nullptr;
			return &(it->second);
		}

		void DrawText(const char* txt, glm::vec2 pos, float scale = 1.0f);
		void QueueText(RenderQueue& queue, const char* txt, glm::vec2 pos, float scale = 1.0f);
		float MeasureText(const char* txt, float scale = 1.0f);

		void DrawText(const wchar_t* txt, glm::vec2 pos, float scale = 1.0f);
		void QueueText(RenderQueue& queue, const wchar_t* txt, glm::vec2 pos, float scale = 1.0f);
		float MeasureText(const wchar_t* txt, float scale = 1.0f);

		auto baseline() { return common.base; }
		auto lineHeight() { return common.lineHeight; }
		auto size() { return info.fontSize; }
		auto scaleForSize(float size) { return  (float)size / (float)this->size(); }
	protected:
		template<typename Text, typename Func>
		float GenericDraw(Func &f, const Text* txt, glm::vec2 pos, float scale = 1.0f)
		{
			uint32_t prev = 0;
			auto current = MX::utf8::next_character(txt);
			while (current != 0)
			{
				auto c = character(current);
				if (!c)  c = character('?');
				if (c)
				{
					auto& info = *(c->info());
					auto kerning = kernings.kerning(prev, current) * scale;
					pos.x += kerning;

					auto p = pos;
					p += glm::vec2(info.xoffset, info.yoffset) * scale;
					
					f(c, glm::round(p), scale);
					pos.x += info.xadvance * scale;

					prev = current;
				}
				current = MX::utf8::next_character(txt);
			}
			return pos.x;
		}

		void load_atlases();

		std::string _basepath;
		bmfont::Info   info;
		bmfont::Common common;
		bmfont::Pages  pages;
		bmfont::Chars  chars;
		bmfont::Kernings kernings;

		using Atlas = std::shared_ptr<MX::Graphic::TextureImage>;
		std::vector<Atlas> _atlases;
		std::map<uint32_t, Character> _characters;
	};

	class BitmapFontSized
	{
	public:
		using pointer = std::shared_ptr<BitmapFontSized>;

		BitmapFontSized()
		{

		}

		static pointer Create(const std::string& path, float size)
		{
			auto parentFont = std::make_shared<BitmapFont>();
			if (!parentFont->load(path)) return nullptr;

			auto ptr = std::make_shared<BitmapFontSized>();
			ptr->_parentFont = parentFont;
			ptr->_size = size;
			return ptr;
		}

		auto& parentFont() { return _parentFont; }
		auto size() { return _size; }
	protected:
		BitmapFont::pointer _parentFont;
		float               _size = 16.0f;
	};
}

namespace MX
{
	template<>
	struct PropertyLoader<MX::Graphic::BitmapFont::pointer>
	{
		using type = PropertyLoader_Custom;
		static bool load(MX::Graphic::BitmapFont::pointer& out, const MX::Scriptable::Value &value)
		{
			ScriptObjectString script(value.fullPath());

			std::string name;
			if (script.load_property_child(name, "Name"))
			{
				out = Resources::get().loadBitmapFont(name);
				return true;
			}
			return false;
		}
	};

	template<>
	struct PropertyLoader<MX::Graphic::BitmapFontSized::pointer>
	{
		using type = PropertyLoader_Custom;
		static bool load(MX::Graphic::BitmapFontSized::pointer& out, const MX::Scriptable::Value &value)
		{
			ScriptObjectString script(value.fullPath());

			std::string name;
			float size = 10.0f;
			if (script.load_property_child(name, "Name"))
			{
				script.load_property(size, "Size");
				out = MX::Graphic::BitmapFontSized::Create(name, size);
				return true;
			}
			return false;
		}
	};
}