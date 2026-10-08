#include "SvgIcon.hpp"
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#define NANOSVG_IMPLEMENTATION
#include "vendor/nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "vendor/nanosvgrast.h"

using namespace geode::prelude;

float gdui::screenPixelScale() {
    GLint viewport[4] = {};
    glGetIntegerv(GL_VIEWPORT, viewport);
    auto win = CCDirector::sharedDirector()->getWinSize();
    if (viewport[2] > 0 && win.width > 0.f) return viewport[2] / win.width;
    return std::max(1.f, CCDirector::sharedDirector()->getContentScaleFactor());
}

CCTexture2D* gdui::renderSvg(std::string source, int pixels) {
    if (source.size() > 65536 || pixels < 8 || pixels > 2048) return nullptr;
    size_t position = 0;
    while ((position = source.find("currentColor", position)) != std::string::npos)
        source.replace(position, 12, "#ffffff");
    std::unique_ptr<NSVGimage, decltype(&nsvgDelete)> image(
        nsvgParse(source.data(), "px", 96.f), nsvgDelete);
    if (!image || image->width <= 0.f || image->height <= 0.f) return nullptr;
    std::unique_ptr<NSVGrasterizer, decltype(&nsvgDeleteRasterizer)> rasterizer(
        nsvgCreateRasterizer(), nsvgDeleteRasterizer);
    if (!rasterizer) return nullptr;
    std::vector<unsigned char> data(pixels * pixels * 4);
    auto scale = pixels / std::max(image->width, image->height);
    nsvgRasterize(rasterizer.get(), image.get(), 0.f, 0.f, scale,
                  data.data(), pixels, pixels, pixels * 4);
    auto texture = new CCTexture2D();
    if (!texture->initWithData(data.data(), kCCTexture2DPixelFormat_RGBA8888,
                              pixels, pixels, {static_cast<float>(pixels), static_cast<float>(pixels)})) {
        delete texture;
        return nullptr;
    }
    // SVG coverage is already antialiased at the destination resolution.
    texture->setAliasTexParameters();
    texture->autorelease();
    return texture;
}
