#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "ui/TopBar.hpp"
#include "ui/UIToggle.hpp"
#include "ui/RadialMenu.hpp"

using namespace geode::prelude;

class $modify(GDUIMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        gdui::ensureUIToggle();
        if (!Mod::get()->getSettingValue<bool>("enabled")) return true;
        if (getChildByID("top-bar"_spr)) return true;

        if (auto bar = gdui::TopBar::create(this)) {
            bar->setVisible(gdui::newUIEnabled());
            addChild(bar, 100);
        }
        else log::warn("Unable to create the GD UI top bar");
        if (auto radial = gdui::RadialMenu::create(this)) addChild(radial, 99);
        else log::warn("Keeping original menu: radial menu could not be built");
        return true;
    }
};
