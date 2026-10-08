#include "UIToggle.hpp"
#include <Geode/ui/OverlayManager.hpp>
using namespace geode::prelude;

namespace {
    void applyMode(CCNode* node, bool enabled) {
        if (!node) return;
        if (node->getID() == "top-bar"_spr) node->setVisible(enabled);
        if (auto children = node->getChildren())
            for (auto child : CCArrayExt<CCNode*>(children)) applyMode(child, enabled);
    }
    CCNode* image(ccColor4B background, char const* title) {
        auto root = CCNode::create();
        root->setContentSize({108, 32});
        root->addChild(CCLayerColor::create({125, 211, 252, 255}, 108, 32));
        auto fill = CCLayerColor::create(background, 106, 30);
        fill->setPosition({1, 1});
        root->addChild(fill);
        auto label = CCLabelBMFont::create(title, "bigFont.fnt");
        label->setScale(.32f);
        label->setPosition({54, 16});
        root->addChild(label);
        return root;
    }
    class UIToggle final : public CCMenu {
        CCMenuItemSprite* m_button = nullptr;
        void updateLabel() {
            auto title = gdui::newUIEnabled() ? "USE OG UI" : "USE NEW UI";
            m_button->setNormalImage(image({22, 25, 31, 255}, title));
            m_button->setSelectedImage(image({48, 65, 80, 255}, title));
        }
    public:
        static UIToggle* create() {
            auto menu = new UIToggle();
            if (menu->init()) { menu->autorelease(); return menu; }
            delete menu;
            return nullptr;
        }
        bool init() override {
            if (!CCMenu::init()) return false;
            setID("ui-toggle"_spr);
            setAnchorPoint({0, 0});
            setPosition({0, 0});
            auto size = CCDirector::sharedDirector()->getWinSize();
            setContentSize(size);
            m_button = CCMenuItemSprite::create(image({22, 25, 31, 255}, "USE OG UI"),
                image({48, 65, 80, 255}, "USE OG UI"), this, menu_selector(UIToggle::toggle));
            m_button->setPosition({size.width - 62, 24});
            addChild(m_button);
            updateLabel();
            return true;
        }
        void registerWithTouchDispatcher() override {
            // Register only after entering the overlay. Never change an
            // unregistered handler or modify the game's scheduler.
            CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -10000, true);
        }
        void toggle(CCObject*) {
            auto enabled = !gdui::newUIEnabled();
            Mod::get()->setSavedValue("new-ui", enabled);
            applyMode(CCDirector::sharedDirector()->getRunningScene(), enabled);
            updateLabel();
        }
    };
}
bool gdui::newUIEnabled() { return Mod::get()->getSavedValue<bool>("new-ui", true); }
void gdui::ensureUIToggle() {
    auto overlay = OverlayManager::get();
    if (overlay->getChildByID("ui-toggle"_spr)) return;
    if (auto menu = UIToggle::create()) overlay->addChild(menu, 100);
}
