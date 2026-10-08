#include "UIToggle.hpp"
#include <limits>
using namespace geode::prelude;

namespace {
    void applyMode(CCNode* node, bool enabled) {
        if (!node) return;
        if (node->getID() == "top-bar"_spr) node->setVisible(enabled);
        if (!node->getChildren()) return;
        for (auto child : CCArrayExt<CCNode*>(node->getChildren())) applyMode(child, enabled);
    }

    class UIToggle final : public CCMenu {
        CCLabelBMFont* m_label = nullptr;
    public:
        static UIToggle* create() {
            auto result = new UIToggle();
            if (result->init()) { result->autorelease(); return result; }
            delete result;
            return nullptr;
        }
        bool init() override {
            if (!CCMenu::init()) return false;
            setID("ui-toggle"_spr);
            setPosition({0, 0});
            auto normal = CCLayerColor::create({22, 25, 31, 245}, 58, 20);
            auto selected = CCLayerColor::create({48, 55, 66, 255}, 58, 20);
            auto item = CCMenuItemSprite::create(normal, selected, this, menu_selector(UIToggle::toggle));
            m_label = CCLabelBMFont::create("", "bigFont.fnt");
            m_label->setScale(.23f);
            m_label->setColor({209, 219, 232});
            m_label->setPosition({29, 10});
            item->addChild(m_label);
            addChild(item);
            schedule(schedule_selector(UIToggle::refresh));
            refresh(0);
            return true;
        }
        void registerWithTouchDispatcher() override {
            // Keep this small control usable even while a modal is open.
            CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -10000, true);
        }
        void refresh(float) {
            auto size = CCDirector::sharedDirector()->getWinSize();
            static_cast<CCNode*>(getChildren()->objectAtIndex(0))->setPosition({size.width - 35, 16});
            m_label->setString(gdui::newUIEnabled() ? "OG UI" : "NEW UI");
        }
        void toggle(CCObject*) {
            auto enabled = !gdui::newUIEnabled();
            Mod::get()->setSavedValue("new-ui", enabled);
            applyMode(CCDirector::sharedDirector()->getRunningScene(), enabled);
            refresh(0);
        }
    };
}

bool gdui::newUIEnabled() {
    return Mod::get()->getSavedValue<bool>("new-ui", true);
}
void gdui::addUIToggle(CCScene* scene) {
    if (!scene || scene->getChildByID("ui-toggle"_spr)) return;
    if (auto toggle = UIToggle::create()) scene->addChild(toggle, std::numeric_limits<int>::max());
}
