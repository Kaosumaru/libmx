#pragma once
#include "utils/Utils.h"
#include "utils/Vector2.h"
#include <map>

namespace MX{

struct Quad;

namespace Graphic
{

class TextureImage;

//WIP
class TextureRental : public Singleton<TextureRental>
{
public:
	using TexturePtr = std::shared_ptr<TextureImage>;
	using RentDataInfo = std::tuple<float, float, bool>;
protected:
	struct RentData
	{
		RentData(TexturePtr&& d, RentDataInfo&& i) : data(std::move(d)), info(std::move(i))
		{

		}

		RentData(const RentData& other) = delete;
		RentData(RentData&& other)
		{
			data = std::move(other.data);
			info = std::move(other.info);
		}

		TexturePtr data;
		RentDataInfo info;
	};

	class RentGuard
	{
	public:
		RentGuard() = delete;
		RentGuard(const RentGuard& other) = delete;
		RentGuard(RentGuard&& other) : _data(std::move(other._data))
		{
			std::swap(_rental, other._rental);
		}

		RentGuard(TextureRental& rental, RentData&& data) : _rental(&rental), _data(std::move(data))
		{

		}

		~RentGuard()
		{
			ReturnData();
		}

		const auto &surface() { return _data.data; }
	protected:
		void ReturnData()
		{
			if (_rental)
				_rental->ReturnData(std::move(_data));
		}

		TextureRental* _rental = nullptr;
		RentData _data;
	};

	friend class RentGuard;
public:
	

	TextureRental();
	virtual ~TextureRental();


	RentGuard rentTexture(const glm::vec2& minSize, bool hasAlpha = true);

protected:
	void ReturnData(RentData&& data);


	std::multimap<RentDataInfo, RentData> _availableRentals;
};

}
}
