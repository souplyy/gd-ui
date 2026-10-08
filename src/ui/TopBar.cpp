#include "TopBar.hpp"

#include <Geode/utils/file.hpp>
#include <cmath>
#include <vector>

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
                          ccColor4B foreground, std::vector<std::vector<CCPoint>> const& paths) {
        auto image = CCNode::create();
        image->setContentSize({buttonSize, buttonSize});
        image->addChild(CCLayerColor::create(background, buttonSize, buttonSize));
        auto icon = CCDrawNode::create();
        auto ink = ccColor4F{foreground.r / 255.f, foreground.g / 255.f,
                            foreground.b / 255.f, 1.f};
        float scale = iconSize / 24.f;
        float padding = (buttonSize - iconSize) / 2.f;
        for (auto const& path : paths) {
            for (size_t i = 1; i < path.size(); ++i) {
                auto transform = [=](CCPoint p) {
                    return CCPoint{padding + p.x * scale, padding + (24.f - p.y) * scale};
                };
                // Lucide's 2-unit stroke, round caps and joins.
                icon->drawSegment(transform(path[i - 1]), transform(path[i]), scale, ink);
            }
        }
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
    if (!number(config["height"], 16.f, 48.f, height) ||
        !number(config["button_size"], 14.f, height, button) ||
        !number(config["icon_size"], 8.f, button, icon) ||
        !number(config["left_padding"], 0.f, 48.f, left) ||
        !color(config["background"], background) || !color(config["pressed"], pressed) ||
        !color(config["foreground"], foreground) || !color(config["divider"], divider)) return false;

    std::vector<std::vector<CCPoint>> paths;
    auto input = config["settings_paths"].asArray();
    if (!input || input.unwrap().empty() || input.unwrap().size() > 32) return false;
    for (auto const& path : input.unwrap()) {
        auto points = path.asArray();
        if (!points || points.unwrap().size() < 2 || points.unwrap().size() > 512) return false;
        std::vector<CCPoint> line;
        for (auto const& point : points.unwrap()) {
            auto xy = point.asArray();
            float x, y;
            if (!xy || xy.unwrap().size() != 2 ||
                !number(xy.unwrap()[0], 0.f, 24.f, x) ||
                !number(xy.unwrap()[1], 0.f, 24.f, y)) return false;
            line.emplace_back(x, y);
        }
        paths.push_back(std::move(line));
    }

    // Build the replacement before removing the working controls.
    auto controls = TopBarMenu::create();
    if (!controls) return false;
    auto settings = CCMenuItemSprite::create(
        settingsImage(button, icon, background, foreground, paths),
        settingsImage(button, icon, pressed, foreground, paths),
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
