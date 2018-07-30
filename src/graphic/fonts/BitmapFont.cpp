#include "BitmapFont.h"
#include "game/resources/Resources.h"
#include <iostream>
#include "graphic/opengl/Uniform.h"
#include "graphic/opengl/ProgramInstance.h"
#include "graphic/renderer/MVP.h"
#include "graphic/renderer/InstancedRenderer.h"
#include "graphic/renderer/DefaultRenderers.h"
#include "graphic/opengl/Texture.h"

using namespace MX::Graphic;

namespace MX::Graphic::bmfont
{
	void Header::Read(std::ifstream& file)
	{
		file.read((char*)&type, sizeof(type));
		file.read((char*)&length, sizeof(length));
	}

	bool Info::Read(std::ifstream& file)
	{
		Header header;
		header.Read(file);

		if (header.type != type) return false;

		constexpr int static_size = 14;
		Info info;
		file.read((char*)this, static_size);

		auto s = header.length - static_size;
		fontName.resize(s);
		file.read(fontName.data(), s);
		return true;
	}

	bool Common::Read(std::ifstream& file)
	{
		Header header;
		header.Read(file);

		if (header.type != type) return false;
		file.read((char*)this, static_size);
		return true;
	}

	bool Pages::Read(std::ifstream& file)
	{
		Header header;
		header.Read(file);
		if (header.type != type) return false;

		data.reset(new char[header.length]);
		file.read(data.get(), header.length);

		page_len = strlen(data.get()) + 1;
		pages = header.length / page_len;
		return true;
	}

	bool Chars::Read(std::ifstream& file)
	{
		Header header;
		header.Read(file);
		if (header.type != type) return false;

		data.reset(new char[header.length]);
		file.read(data.get(), header.length);

		int size = header.length / Char::size;

		for (int i = 0; i < size; i++)
		{
			auto c = character_for_index(i);
			id_chars[c->id] = c;
		}

		return true;
	}

	bool Kernings::Read(std::ifstream& file)
	{
		Header header;
		header.Read(file);
		if (header.type != type) return false;

		constexpr int static_size = 10;
		int size = header.length / static_size;

		for (int i = 0; i < size; i++)
		{
			uint32_t f;
			uint32_t s;
			uint16_t a;

			file.read((char*)&f, sizeof(f));
			file.read((char*)&s, sizeof(s));
			file.read((char*)&a, sizeof(a));

			kernings[std::make_pair(f, s)] = a;
		}

		return true;
	}

}

bool BitmapFont::load(const std::string& path)
{
	auto abs_path = MX::Paths::get().pathToResource(path);

	std::ifstream file(abs_path.c_str(), std::ios::in | std::ios::binary);
	if (!file) return false;

	char magic[4] = {0};
	file.read(magic, 3);
	char format = 0;
	file.read(&format, 1);


	if (!info.Read(file))   return false;
	if (!common.Read(file)) return false;
	if (!pages.Read(file))  return false;
	if (!chars.Read(file))  return false;
	if (!kernings.Read(file))  return false;

	_basepath = path.substr(0, path.find_last_of('/') + 1);
	load_atlases();
	return true;
}

void BitmapFont::load_atlases()
{
	gl::Texture::LoadInfo info;
	info.setupMipMap();
	auto l = Context<gl::Texture::LoadInfo>::Lock(info);

	for (int i = 0; i < pages.pages; i++)
	{
		auto path = _basepath + pages.page(i);
		auto img = MX::Resources::get().loadImage(path);
		_atlases.push_back(img);
	}


	for (auto &p : chars.characters())
	{
		auto c = p.second;
		auto& res = _characters[c->id];
		res._info = c;

		auto& atlas = _atlases[c->page];
		res._image = MX::Graphic::TextureImage::Create(atlas, c->x, c->y, c->width, c->height);
	}
}


void BitmapFont::DrawText(const char* txt, glm::vec2 pos, float scale)
{
	auto func = [](auto c, const glm::vec2 &p, float scale)
	{
		c->image()->DrawScaled({}, p, {scale, scale});
	};

	GenericDraw(func, txt, pos, scale);
}


void BitmapFont::QueueText(BitmapFontRenderQueue& queue, const char* txt, glm::vec2 pos, float scale)
{
	auto func = [&queue](auto c, const glm::vec2 &p, float scale)
	{
		BitmapFontRenderQueue::Item item;
		item.glyph = c->image();
		item.pos = p;
		item.scale = scale;
		queue.AddItem(item);
	};

	GenericDraw(func, txt, pos, scale);
}

int BitmapFont::MeasureText(const char* txt, float scale)
{
	auto func = [](auto c, const glm::vec2 &p, float scale)
	{
		
	};
	return GenericDraw(func, txt, {}, scale);
}

void BitmapFont::DrawText(const wchar_t* txt, glm::vec2 pos, float scale)
{
	auto func = [](auto c, const glm::vec2 &p, float scale)
	{
		c->image()->DrawScaled({}, p, {scale, scale});
	};

	GenericDraw(func, txt, pos, scale);
}


void BitmapFont::QueueText(BitmapFontRenderQueue& queue, const wchar_t* txt, glm::vec2 pos, float scale)
{
	auto func = [&queue](auto c, const glm::vec2 &p, float scale)
	{
		BitmapFontRenderQueue::Item item;
		item.glyph = c->image();
		item.pos = p;
		item.scale = scale;
		queue.AddItem(item);
	};

	GenericDraw(func, txt, pos, scale);
}

int BitmapFont::MeasureText(const wchar_t* txt, float scale)
{
	auto func = [](auto c, const glm::vec2 &p, float scale)
	{

	};
	return GenericDraw(func, txt, {}, scale);
}

