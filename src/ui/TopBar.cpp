#include "TopBar.hpp"

#include <array>
#include <cmath>
#include <numbers>

using namespace geode::prelude;

namespace {
    class TopBarMenu final : public CCMenu {
    public:
        static TopBarMenu* create() {
            auto menu = new TopBarMenu();
            if (menu->init()) {
                menu->autorelease();
                return menu;
            }
            delete menu;
            return nullptr;
        }

        void registerWithTouchDispatcher() override {
            // Register at the desired priority, rather than changing a handler
            // that does not exist until this menu enters the running scene.
            CCDirector::sharedDirector()->getTouchDispatcher()
                ->addPrioTargetedDelegate(this, -131, true);
        }
    };

    constexpr float barHeight = 44.f;
    constexpr float buttonSize = 34.f;
    constexpr ccColor4B barColor = {22, 25, 31, 255};

    CCNode* settingsImage(bool pressed) {
        auto node = CCNode::create();
        node->setContentSize({buttonSize, buttonSize});

        auto backgroundColor = pressed ? ccColor4B{48, 55, 66, 255} : barColor;
        auto background = CCLayerColor::create(backgroundColor, buttonSize, buttonSize);
        node->addChild(background);

        // An authored flat gear avoids the game's glossy, outlined sprites.
        auto gear = CCDrawNode::create();
        constexpr unsigned int segments = 48;
        std::array<CCPoint, segments> outline;
        for (unsigned int i = 0; i < segments; ++i) {
            float angle = 2.f * std::numbers::pi_v<float> * i / segments;
            float radius = (i % 6 >= 1 && i % 6 <= 3) ? 10.f : 7.6f;
            outline[i] = CCPoint{17.f + std::cos(angle) * radius,
                          17.f + std::sin(angle) * radius};
        }
        gear->drawPolygon(outline.data(), segments,
                          {0.82f, 0.86f, 0.91f, 1.f}, 0.f, {0.f, 0.f, 0.f, 0.f});
        gear->drawDot({17.f, 17.f}, 3.3f,
                      {backgroundColor.r / 255.f, backgroundColor.g / 255.f,
                       backgroundColor.b / 255.f, 1.f});
        node->addChild(gear);
        return node;
    }
}

bool gdui::TopBar::init(MenuLayer* menuLayer) {
    auto size = CCDirector::sharedDirector()->getWinSize();
    if (!CCLayerColor::initWithColor(barColor, size.width, barHeight)) return false;
    setID("top-bar"_spr);
    setPosition({0.f, size.height - barHeight});
    setTouchEnabled(true);

    auto divider = CCLayerColor::create({43, 49, 59, 255}, size.width, 1.f);
    divider->setID("divider"_spr);
    addChild(divider);

    auto controls = TopBarMenu::create();
    if (!controls) return false;
    controls->setID("top-bar-controls"_spr);
    controls->setPosition({0.f, 0.f});
    controls->setContentSize({size.width, barHeight});
    auto settings = CCMenuItemSprite::create(
        settingsImage(false), settingsImage(true), menuLayer,
        menu_selector(MenuLayer::onOptions)
    );
    settings->setID("settings-button"_spr);
    settings->setPosition({27.f, barHeight / 2.f});
    controls->addChild(settings);
    addChild(controls);
    return true;
}

void gdui::TopBar::registerWithTouchDispatcher() {
    CCDirector::sharedDirector()->getTouchDispatcher()->addPrioTargetedDelegate(this, -130, true);
}

bool gdui::TopBar::ccTouchBegan(CCTouch* touch, CCEvent*) {
    // Empty bar space must not activate controls hidden behind it.
    auto point = convertToNodeSpace(touch->getLocation());
    return CCRect{0.f, 0.f, getContentSize().width, barHeight}.containsPoint(point);
}

gdui::TopBar* gdui::TopBar::create(MenuLayer* menuLayer) {
    auto bar = new TopBar();
    if (bar->init(menuLayer)) {
        bar->autorelease();
        return bar;
    }
    delete bar;
    return nullptr;
}
