#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/utils/cocos.hpp>

#include "ui/StarterPopup.hpp"

using namespace geode::prelude;

class $modify(GDUIMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        if (!Mod::get()->getSettingValue<bool>("enabled")) return true;

        // Use the shared menu and its layout so other mods can add buttons too.
        auto menu = typeinfo_cast<CCMenu*>(getChildByID("bottom-menu"));
        if (!menu) {
            log::warn("Main menu has no bottom-menu; skipping the GD UI button");
            return true;
        }
        if (menu->getChildByID("open-button"_spr)) return true;

        auto sprite = CircleButtonSprite::createWithSpriteFrameName(
            "GJ_infoIcon_001.png", 1.f, CircleBaseColor::Cyan, CircleBaseSize::Medium
        );
        if (!sprite) {
            log::warn("Unable to create the GD UI button sprite");
            return true;
        }

        auto button = CCMenuItemExt::createSpriteExtra(sprite, [](auto*) {
            if (auto popup = gdui::StarterPopup::create()) popup->show();
            else log::error("Unable to create the GD UI popup");
        });
        button->setID("open-button"_spr);
        menu->addChild(button);
        menu->updateLayout();
        return true;
    }
};
