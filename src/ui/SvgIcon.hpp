#pragma once
#include <Geode/Geode.hpp>
namespace gdui {
    cocos2d::CCTexture2D* renderSvg(std::string source, int pixels);
    float screenPixelScale();
}
