#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "ui/TopBar.hpp"

using namespace geode::prelude;

class $modify(GDUIMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        if (!Mod::get()->getSettingValue<bool>("enabled")) return true;
        if (getChildByID("top-bar"_spr)) return true;

        if (auto bar = gdui::TopBar::create(this)) addChild(bar, 100);
        else log::warn("Unable to create the GD UI top bar");
        return true;
    }
};
