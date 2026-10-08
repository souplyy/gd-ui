#include "TopBar.hpp"

#include <Geode/utils/file.hpp>
#include <cmath>
#include <Geode/utils/string.hpp>

using namespace geode::prelude;

namespace {
    class TopBarMenu final : public CCMenu {
    public:
        static TopBarMenu* create() {
            auto menu = new TopBarMenu();
            if (menu->init()) { menu->autorelease(); return menu; }
            delete menu;
            return nullptr;
        }
        void registerWithTouchDispatcher() override {
            CCDirector::sharedDirector()->getTouchDispatcher()
                ->addPrioTargetedDelegate(this, -131, true);
        }
        bool tracking() const { return m_eState == kCCMenuStateTrackingTouch; }
    };

    bool number(matjson::Value const& value, float low, float high, float& output) {
        auto parsed = value.asDouble();
        if (!parsed) return false;
        auto n = parsed.unwrap();
        if (!std::isfinite(n) || n < low || n > high) return false;
        output = static_cast<float>(n);
        return true;
    }

    bool color(matjson::Value const& value, ccColor4B& output) {
        auto array = value.asArray();
        if (!array || array.unwrap().size() != 3) return false;
        float rgb[3];
        for (int i = 0; i < 3; ++i)
            if (!number(array.unwrap()[i], 0.f, 255.f, rgb[i])) return false;
        output = {static_cast<GLubyte>(rgb[0]), static_cast<GLubyte>(rgb[1]),
                  static_cast<GLubyte>(rgb[2]), 255};
        return true;
    }

    CCNode* settingsImage(float buttonSize, float iconSize, ccColor4B background,
                          ccColor4B foreground, CCTexture2D* texture) {
        auto image = CCNode::create();
        image->setContentSize({buttonSize, buttonSize});
        image->addChild(CCLayerColor::create(background, buttonSize, buttonSize));
        auto icon = CCSprite::createWithTexture(texture);
        if (!icon) return nullptr;
        icon->setColor({foreground.r, foreground.g, foreground.b});
        icon->setScale(iconSize / icon->getContentSize().width);
        icon->setPosition({buttonSize / 2.f, buttonSize / 2.f});
        image->addChild(icon);
        return image;
    }
}

bool gdui::TopBar::apply(std::string const& content) {
    if (content.size() > 65536) return false;
    auto parsed = matjson::parse(content);
    if (!parsed) return false;
    auto const& config = parsed.unwrap();
    float height, button, icon, left;
    ccColor4B background, pressed, foreground, divider;
    if (!number(config["height"], 10.f, 48.f, height) ||
        !number(config["button_size"], 10.f, height, button) ||
        !number(config["icon_size"], 8.f, button, icon) ||
        !number(config["left_padding"], 0.f, 48.f, left) ||
        !color(config["background"], background) || !color(config["pressed"], pressed) ||
        !color(config["foreground"], foreground) || !color(config["divider"], divider)) return false;

    auto iconPath = Mod::get()->getConfigDir() / "settings.png";
    std::error_code error;
    if (!std::filesystem::exists(iconPath, error))
        iconPath = Mod::get()->getResourcesDir() / "settings.png";
    auto filename = string::pathToString(iconPath);
    auto cache = CCTextureCache::sharedTextureCache();
    cache->removeTextureForKey(filename.c_str());
    auto texture = cache->addImage(filename.c_str(), true);
    if (!texture) return false;

    // Build the replacement before removing the working controls.
    auto controls = TopBarMenu::create();
    if (!controls) return false;
    auto settings = CCMenuItemSprite::create(
        settingsImage(button, icon, background, foreground, texture),
        settingsImage(button, icon, pressed, foreground, texture),
        m_owner, menu_selector(MenuLayer::onOptions)
    );
    if (!settings) return false;
    auto size = CCDirector::sharedDirector()->getWinSize();
    controls->setID("top-bar-controls"_spr);
    controls->setPosition({0.f, 0.f});
    controls->setContentSize({size.width, height});
    settings->setID("settings-button"_spr);
    settings->setPosition({left + button / 2.f, height / 2.f});
    controls->addChild(settings);

    removeAllChildrenWithCleanup(true);
    m_height = height;
    setColor({background.r, background.g, background.b});
    setContentSize({size.width, height});
    setPosition({0.f, size.height - height});
    auto line = CCLayerColor::create(divider, size.width, .5f);
    line->setID("divider"_spr);
    addChild(line);
    addChild(controls);
    m_controls = controls;
    return true;
}

bool gdui::TopBar::init(MenuLayer* menuLayer) {
    if (!CCLayerColor::initWithColor({22, 25, 31, 255})) return false;
    m_owner = menuLayer;
    setID("top-bar"_spr);
    setTouchEnabled(true);
    auto bundled = file::readString(Mod::get()->getResourcesDir() / "ui.json");
    if (!bundled || !apply(bundled.unwrap())) return false;
    auto path = Mod::get()->getConfigDir() / "ui.json";
    std::error_code error;
    if (!std::filesystem::exists(path, error)) {
        auto written = file::writeStringSafe(path, bundled.unwrap());
        if (!written) log::warn("Unable to create live UI config: {}", written.unwrapErr());
    }
    refresh(0.f);
    schedule(schedule_selector(TopBar::refresh), .5f);
    return true;
}

void gdui::TopBar::refresh(float) {
    // Do not register new controls above an active popup's touch priority.
    if (CCDirector::sharedDirector()->getTouchDispatcher()->isUsingForcePrio()) return;
    if (m_controls && static_cast<TopBarMenu*>(m_controls)->tracking()) return;
    auto content = file::readString(Mod::get()->getConfigDir() / "ui.json");
    if (!content || content.unwrap() == m_lastContent) return;
    m_lastContent = content.unwrap();
    if (apply(m_lastContent)) log::info("Live UI refreshed");
    else log::warn("Invalid live UI config; keeping the previous top bar");
}

void gdui::TopBar::registerWithTouchDispatcher() {
    CCDirector::sharedDirector()->getTouchDispatcher()->addPrioTargetedDelegate(this, -130, true);
}

bool gdui::TopBar::ccTouchBegan(CCTouch* touch, CCEvent*) {
    auto point = convertToNodeSpace(touch->getLocation());
    return CCRect{0.f, 0.f, getContentSize().width, m_height}.containsPoint(point);
}

gdui::TopBar* gdui::TopBar::create(MenuLayer* menuLayer) {
    auto bar = new TopBar();
    if (bar->init(menuLayer)) { bar->autorelease(); return bar; }
    delete bar;
    return nullptr;
}
