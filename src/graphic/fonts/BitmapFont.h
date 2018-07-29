#pragma once
#include <memory>
#include <vector>
#include <map>
#include <fstream>
#include "graphic/images/TextureImage.h"


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

	class BitmapFontRenderQueue
	{
	public:
		struct Item
		{
			std::shared_ptr<MX::Graphic::TextureImage> glyph;
			glm::vec2 pos;
			float scale = 1.0f;
			void Render()
			{
				glyph->DrawScaled({}, pos, {scale, scale});
			}
		};

		using Queue = std::vector<Item>;

		BitmapFontRenderQueue()
		{

		}

		BitmapFontRenderQueue(const BitmapFontRenderQueue& other) : _queue(other._queue)
		{
		}

		BitmapFontRenderQueue(BitmapFontRenderQueue&& other) : _queue(std::move(other._queue))
		{
		}

		void AddItem(const Item& item)
		{
			_queue.push_back(item);
		}

		void Render()
		{
			for (auto& item : _queue)
			{
				item.Render();
			}
		}
	protected:
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
		void QueueText(BitmapFontRenderQueue& queue, const char* txt, glm::vec2 pos, float scale = 1.0f);
		int MeasureText(const char* txt, float scale = 1.0f);

	protected:
		template<typename Func>
		int GenericDraw(Func &f, const char* txt, glm::vec2 pos, float scale = 1.0f)
		{
			uint32_t prev = 0;
			uint32_t current = 0;
			while (*txt != 0)
			{
				current = *txt;
				if (auto c = character(current))
				{
					auto& info = *(c->info());
					auto p = pos;
					p += glm::vec2(info.xoffset, info.yoffset) * scale;
					p.x += kernings.kerning(prev, current) * scale;
					f(c, p, scale);
					pos.x += info.xadvance * scale;

					prev = current;
				}
				txt++;
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


}