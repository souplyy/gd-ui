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
CCNode* roundedBox(float width, float height, float radius, char const* fill, bool border=false) {
    auto node = CCNode::create();
    node->setContentSize({width,height});
    // Use a square texture with the rounded rectangle at its bottom, keeping
    // the texture scale identical on both axes and the hit box compact.
    auto svg = fmt::format("<svg xmlns='http://www.w3.org/2000/svg' width='{0}' height='{0}'><rect x='.5' y='{1}' width='{2}' height='{3}' rx='{4}' fill='{5}' stroke='{6}' stroke-width='1'/></svg>",
        width, width-height+.5f, width-1, height-1, radius, fill, border ? "#465463" : fill);
    if (auto texture = gdui::renderSvg(svg, int(std::round(width*gdui::screenPixelScale())))) {
        auto sprite = CCSprite::createWithTexture(texture);
        sprite->setAnchorPoint({0,0}); sprite->setPosition({0,0});
        sprite->setScale(width/sprite->getContentSize().width);
        sprite->setOpacityModifyRGB(false); sprite->setBlendFunc({GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA});
        node->addChild(sprite);
    }
    return node;
}
CCNode* row(char const* text, float width, float height, bool pressed) {
    auto node = roundedBox(width,height,4,pressed ? "#303742" : "#16191f");
    auto label = CCLabelBMFont::create(text,"bigFont.fnt");
    label->setColor({209,219,232}); label->setScale(.19f);
    label->setAnchorPoint({0,.5f}); label->setPosition({9,height/2}); node->addChild(label);
    return node;
}
std::string username() {
    auto name = std::string(GameManager::sharedState()->m_playerName);
    return name.empty() ? "Player" : name;
}
CCNode* trigger(float height, bool pressed) {
    auto node = row("",86,height-2,pressed);
    auto gm = GameManager::sharedState();
    auto avatar = SimplePlayer::create(gm->getPlayerFrame());
    avatar->setColor(gm->colorForIdx(gm->getPlayerColor()));
    avatar->setSecondColor(gm->colorForIdx(gm->getPlayerColor2()));
    avatar->setScale(.27f); avatar->setPosition({9,height/2}); node->addChild(avatar);
    auto label = CCLabelBMFont::create(username().c_str(),"bigFont.fnt");
    label->limitLabelWidth(50,.2f,.1f); label->setAnchorPoint({0,.5f});
    label->setPosition({20,height/2}); label->setColor({209,219,232}); node->addChild(label);
    auto svg = file::readString(Mod::get()->getResourcesDir()/"chevron-down.svg");
    if (svg) if (auto texture = gdui::renderSvg(svg.unwrap(),int(std::round(7*gdui::screenPixelScale())))) {
        auto icon = CCSprite::createWithTexture(texture);
        icon->setScale(7/icon->getContentSize().width); icon->setPosition({79,height/2});
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
    button->setPosition({width-49,height/2}); controls->addChild(button); addChild(controls);
    m_panel=CCNode::create(); m_panel->setContentSize({132,88});
    m_panelHome={width-138,-90}; m_panel->setPosition(m_panelHome); m_panel->setVisible(false); addChild(m_panel,1);
    m_panel->addChild(roundedBox(132,88,8,"#16191f",true));
    auto name=CCLabelBMFont::create(username().c_str(),"bigFont.fnt");
    name->limitLabelWidth(112,.24f,.12f); name->setAnchorPoint({0,.5f}); name->setPosition({10,73}); m_panel->addChild(name);
    auto line=CCLayerColor::create({70,84,99,255},112,.5f); line->setPosition({10,60}); m_panel->addChild(line);
    auto items=ProfileControls::create(); m_panelControls=items; m_panel->addChild(items);
    char const* labels[]={"View Profile","Icon Kit","Account"};
    SEL_MenuHandler actions[]={menu_selector(ProfileMenu::profile),menu_selector(ProfileMenu::icons),menu_selector(ProfileMenu::account)};
    for(int i=0;i<3;++i) {
        auto item=CCMenuItemSprite::create(row(labels[i],120,18,false),row(labels[i],120,18,true),this,actions[i]);
        item->setPosition({66,48.f-19*i}); items->addChild(item);
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
void gdui::ProfileMenu::finishClose() {
    m_panel->setVisible(false);
    m_animating=false;
}
void gdui::ProfileMenu::close() {
    if (!m_open) return;
    m_open=false; m_animating=true;
    m_panelControls->setEnabled(false);
    m_panel->stopAllActions();
    m_panel->runAction(CCSequence::create(
        CCEaseSineIn::create(CCMoveTo::create(.09f,m_panelHome+CCPoint{0,8})),
        CCCallFunc::create(this,callfunc_selector(ProfileMenu::finishClose)), nullptr));
}
void gdui::ProfileMenu::toggle(CCObject*) {
    if (m_open) { close(); return; }
    m_panel->stopAllActions();
    if (!m_panel->isVisible()) m_panel->setPosition(m_panelHome+CCPoint{0,10});
    m_panel->setVisible(true); m_open=true; m_animating=false;
    m_panelControls->setEnabled(true);
    m_panel->runAction(CCEaseExponentialOut::create(CCMoveTo::create(.14f,m_panelHome)));
}
void gdui::ProfileMenu::profile(CCObject*) { close(); m_owner->onMyProfile(nullptr); }
void gdui::ProfileMenu::icons(CCObject*) { close(); m_owner->onGarage(nullptr); }
void gdui::ProfileMenu::account(CCObject*) { close(); if(auto layer=AccountLayer::create()) layer->showLayer(false); }

bool gdui::ProfileMenu::isBusy() const {
    return m_open || m_animating || (m_triggerControls && static_cast<ProfileControls*>(m_triggerControls)->tracking())
        || (m_panelControls && static_cast<ProfileControls*>(m_panelControls)->tracking());
}
