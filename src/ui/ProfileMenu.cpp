#include "ProfileMenu.hpp"
#include "SvgIcon.hpp"
#include <Geode/utils/file.hpp>
#include <cmath>
using namespace geode::prelude;
namespace {
class ProfileControls final : public CCMenu {
public:
    static ProfileControls* create() {
        auto menu = new ProfileControls();
        if (menu->init()) { menu->autorelease(); menu->setPosition({0,0}); return menu; }
        delete menu; return nullptr;
    }
    bool tracking() const { return m_eState == kCCMenuStateTrackingTouch; }
    void registerWithTouchDispatcher() override {
        CCDirector::sharedDirector()->getTouchDispatcher()->addPrioTargetedDelegate(this, -133, true);
    }
    bool ccTouchBegan(CCTouch* t, CCEvent* e) override {
        if (CCDirector::sharedDirector()->getTouchDispatcher()->isUsingForcePrio()) return false;
        return CCMenu::ccTouchBegan(t,e);
    }
};
CCNode* row(char const* text, float width, float height, bool pressed) {
    auto node = CCNode::create(); node->setContentSize({width,height});
    node->addChild(CCLayerColor::create(pressed ? ccColor4B{48,55,66,255} : ccColor4B{22,25,31,255},width,height));
    auto label = CCLabelBMFont::create(text,"bigFont.fnt");
    label->setColor({209,219,232}); label->setScale(.22f);
    label->setAnchorPoint({0,.5f}); label->setPosition({12,height/2}); node->addChild(label);
    return node;
}
std::string username() {
    auto name = std::string(GameManager::sharedState()->m_playerName);
    return name.empty() ? "Player" : name;
}
CCNode* trigger(float height, bool pressed) {
    auto node = row("",100,height,pressed);
    auto gm = GameManager::sharedState();
    auto avatar = SimplePlayer::create(gm->getPlayerFrame());
    avatar->setColor(gm->colorForIdx(gm->getPlayerColor()));
    avatar->setSecondColor(gm->colorForIdx(gm->getPlayerColor2()));
    avatar->setScale(.3f); avatar->setPosition({9,height/2}); node->addChild(avatar);
    auto label = CCLabelBMFont::create(username().c_str(),"bigFont.fnt");
    label->limitLabelWidth(64,.22f,.1f); label->setAnchorPoint({0,.5f});
    label->setPosition({20,height/2}); label->setColor({209,219,232}); node->addChild(label);
    auto svg = file::readString(Mod::get()->getResourcesDir()/"chevron-down.svg");
    if (svg) if (auto texture = gdui::renderSvg(svg.unwrap(),int(std::round(7*gdui::screenPixelScale())))) {
        auto icon = CCSprite::createWithTexture(texture);
        icon->setScale(7/icon->getContentSize().width); icon->setPosition({93,height/2});
        icon->setColor({209,219,232}); icon->setOpacityModifyRGB(false);
        icon->setBlendFunc({GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA}); node->addChild(icon);
    }
    return node;
}
}
gdui::ProfileMenu* gdui::ProfileMenu::create(MenuLayer* owner,float width,float height) {
    auto node = new ProfileMenu();
    if (node->init(owner,width,height)) { node->autorelease(); return node; }
    delete node; return nullptr;
}
bool gdui::ProfileMenu::init(MenuLayer* owner,float width,float height) {
    if (!CCLayer::init()) return false;
    m_owner=owner; setID("profile-dropdown"_spr); setPosition({0,0});
    auto controls=ProfileControls::create(); m_triggerControls=controls;
    auto button=CCMenuItemSprite::create(trigger(height,false),trigger(height,true),this,menu_selector(ProfileMenu::toggle));
    button->setPosition({width-56,height/2}); controls->addChild(button); addChild(controls);
    m_panel=CCNode::create(); m_panel->setContentSize({156,110});
    m_panel->setPosition({width-162,-112}); m_panel->setVisible(false); addChild(m_panel,1);
    m_panel->addChild(CCLayerColor::create({70,84,99,255},156,110));
    auto fill=CCLayerColor::create({22,25,31,255},154,108); fill->setPosition({1,1}); m_panel->addChild(fill);
    auto name=CCLabelBMFont::create(username().c_str(),"bigFont.fnt");
    name->limitLabelWidth(132,.3f,.12f); name->setAnchorPoint({0,.5f}); name->setPosition({12,91}); m_panel->addChild(name);
    auto line=CCLayerColor::create({70,84,99,255},132,.5f); line->setPosition({12,74}); m_panel->addChild(line);
    auto items=ProfileControls::create(); m_panelControls=items; m_panel->addChild(items);
    char const* labels[]={"View Profile","Icon Kit","Account"};
    SEL_MenuHandler actions[]={menu_selector(ProfileMenu::profile),menu_selector(ProfileMenu::icons),menu_selector(ProfileMenu::account)};
    for(int i=0;i<3;++i) {
        auto item=CCMenuItemSprite::create(row(labels[i],154,24,false),row(labels[i],154,24,true),this,actions[i]);
        item->setPosition({78,61.f-24*i}); items->addChild(item);
    }
    setTouchEnabled(true); return true;
}
void gdui::ProfileMenu::registerWithTouchDispatcher() {
    CCDirector::sharedDirector()->getTouchDispatcher()->addPrioTargetedDelegate(this,-132,true);
}
bool gdui::ProfileMenu::ccTouchBegan(CCTouch*,CCEvent*) {
    if (!m_open || !getParent()->isVisible() || CCDirector::sharedDirector()->getTouchDispatcher()->isUsingForcePrio()) return false;
    close(); return true;
}
void gdui::ProfileMenu::close() { m_open=false; m_panel->setVisible(false); }
void gdui::ProfileMenu::toggle(CCObject*) { m_open=!m_open; m_panel->setVisible(m_open); }
void gdui::ProfileMenu::profile(CCObject*) { close(); m_owner->onMyProfile(nullptr); }
void gdui::ProfileMenu::icons(CCObject*) { close(); m_owner->onGarage(nullptr); }
void gdui::ProfileMenu::account(CCObject*) { close(); if(auto layer=AccountLayer::create()) layer->showLayer(false); }

bool gdui::ProfileMenu::isBusy() const {
    return m_open || (m_triggerControls && static_cast<ProfileControls*>(m_triggerControls)->tracking())
        || (m_panelControls && static_cast<ProfileControls*>(m_panelControls)->tracking());
}
