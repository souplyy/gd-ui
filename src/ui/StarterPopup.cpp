#include "StarterPopup.hpp"

#include <Geode/Geode.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

bool gdui::StarterPopup::init() {
    if (!Popup::init(300.f, 180.f)) return false;
    setID("starter-popup"_spr);
    setTitle("GD UI");

    auto description = CCLabelBMFont::create(
        "Your mod is loaded!\nReady to build a fresh GD interface.", "bigFont.fnt"
    );
    description->setID("description"_spr);
    description->setAlignment(kCCTextAlignmentCenter);
    description->limitLabelWidth(260.f, .45f, .2f);
    m_mainLayer->addChildAtPosition(description, Anchor::Center, {0.f, 8.f});

    auto sprite = ButtonSprite::create("Close");
    sprite->setScale(.7f);
    auto close = CCMenuItemExt::createSpriteExtra(sprite, [this](auto* sender) {
        onClose(sender);
    });
    close->setID("close-button"_spr);
    m_buttonMenu->addChildAtPosition(close, Anchor::Bottom, {0.f, 28.f});
    return true;
}

gdui::StarterPopup* gdui::StarterPopup::create() {
    auto popup = new StarterPopup();
    if (popup->init()) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}
