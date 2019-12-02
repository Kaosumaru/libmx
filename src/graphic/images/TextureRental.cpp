#include "TextureRental.h"
#include "TextureImage.h"

using namespace MX;
using namespace MX::Graphic;

TextureRental::TextureRental()
{
}

TextureRental::~TextureRental()
{
}

TextureRental::RentGuard TextureRental::rentTexture(const glm::vec2& minSize, bool hasAlpha)
{
    auto info = std::make_tuple(minSize.x, minSize.y, hasAlpha);
    auto it = _availableRentals.find(info);
    if (it != _availableRentals.end())
    {
        RentGuard g(*this, std::move(it->second));
        _availableRentals.erase(it);
        return g;
    }

    return RentGuard(*this, RentData(TextureImage::Create(minSize.x, minSize.y, hasAlpha), std::move(info)));
}

void TextureRental::ReturnData(RentData&& data)
{
    _availableRentals.emplace(std::make_pair(data.info, std::move(data)));
}