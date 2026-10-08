#include "RadialMenu.hpp"
#include "RingGeometry.hpp"
#include "SvgIcon.hpp"
#include "UIToggle.hpp"
#include <Geode/utils/file.hpp>
#include <algorithm>
using namespace geode::prelude;

namespace {
    CCSprite* svgSprite(std::string source, float size) {
        auto texture = gdui::renderSvg(source, std::clamp(static_cast<int>(std::round(size * gdui::screenPixelScale())), 8, 2048));
        if (!texture) return nullptr;
        auto sprite = CCSprite::createWithTexture(texture);
        sprite->setScale(size / sprite->getContentSize().width);
        sprite->setOpacityModifyRGB(false);
        sprite->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        return sprite;
    }
}
gdui::RadialMenu* gdui::RadialMenu::create(MenuLayer* owner) {
    auto node = new RadialMenu();
    if (node->init(owner)) { node->autorelease(); return node; }
    delete node;
    return nullptr;
}
bool gdui::RadialMenu::init(MenuLayer* owner) {
    if (!CCLayer::init()) return false;
    char const* ids[] = {"play-button", "icon-kit-button", "stats-button", "achievements-button", "editor-button"};
    for (int i = 0; i < 5; ++i) {
        auto button = typeinfo_cast<CCMenuItem*>(owner->getChildByIDRecursive(ids[i]));
        if (!button) { log::warn("Radial menu missing original {}", ids[i]); return false; }
        m_original[i] = button;
        m_originalVisible[i] = button->isVisible();
    }
    setID("radial-menu"_spr);
    auto size = CCDirector::sharedDirector()->getWinSize();
    m_radius = std::min({116.f, size.height * .33f, size.width * .27f});
    setContentSize({2 * m_radius, 2 * m_radius});
    setPosition({size.width / 2 - m_radius, size.height / 2 - m_radius});
    auto ring = svgSprite(ringSVG(), 2 * m_radius);
    if (!ring) return false;
    ring->setPosition({m_radius, m_radius});
    addChild(ring);
    char const* icons[] = {"play", "shirt", "chart-no-axes-column", "trophy", "compass"};
    char const* labels[] = {"PLAY", "ICONS", "STATS", "ACHIEVEMENTS", "BROWSE"};
    for (int i = 0; i < 5; ++i) {
        auto selected = svgSprite(ringSVG(i), 2 * m_radius);
        if (!selected) return false;
        selected->setPosition({m_radius, m_radius});
        selected->setVisible(false);
        addChild(selected);
        m_highlights[i] = selected;
    }
    for (int i = 0; i < 5; ++i) {
        auto angle = (90 + 72 * i) * pi / 180;
        CCPoint p{m_radius + float(std::cos(angle)) * m_radius * .74f,
                  m_radius + float(std::sin(angle)) * m_radius * .74f};
        auto source = file::readString(Mod::get()->getResourcesDir() / (std::string(icons[i]) + ".svg"));
        if (!source) return false;
        auto icon = svgSprite(source.unwrap(), 20);
        if (!icon) return false;
        icon->setColor({209, 219, 232});
        icon->setPosition(p + CCPoint{0, 6});
        addChild(icon);
        auto label = CCLabelBMFont::create(labels[i], "bigFont.fnt");
        label->setScale(i == 3 ? .13f : .18f);
        label->setColor({209, 219, 232});
        label->setPosition(p + CCPoint{0, -13});
        addChild(label);
    }
    m_player = SimplePlayer::create(GameManager::sharedState()->getPlayerFrame());
    if (!m_player) return false;
    m_player->setPosition({m_radius, m_radius});
    m_player->setScale(1.4f);
    addChild(m_player);
    setTouchEnabled(true);
    return true;
}
void gdui::RadialMenu::onEnter() {
    CCLayer::onEnter();
    auto gm = GameManager::sharedState();
    m_player->updatePlayerFrame(gm->getPlayerFrame(), IconType::Cube);
    m_player->setColor(gm->colorForIdx(gm->getPlayerColor()));
    m_player->setSecondColor(gm->colorForIdx(gm->getPlayerColor2()));
    m_player->setGlowOutline(gm->colorForIdx(gm->getPlayerGlowColor()));
    if (!gm->getPlayerGlow()) m_player->disableGlowOutline();
    setMode(newUIEnabled());
}
void gdui::RadialMenu::setMode(bool enabled) {
    setVisible(enabled);
    highlight(-1);
    m_pressed = -1;
    for (int i = 0; i < 5; ++i) m_original[i]->setVisible(enabled ? false : m_originalVisible[i]);
}
void gdui::RadialMenu::registerWithTouchDispatcher() {
    CCDirector::sharedDirector()->getTouchDispatcher()->addPrioTargetedDelegate(this, -129, true);
}
int gdui::RadialMenu::hit(CCTouch* touch) {
    if (!isVisible() || CCDirector::sharedDirector()->getTouchDispatcher()->isUsingForcePrio()) return -1;
    auto p = convertToNodeSpace(touch->getLocation());
    return ringSector(p.x - m_radius, p.y - m_radius, m_radius * 59 / 120, m_radius * 118 / 120);
}
void gdui::RadialMenu::highlight(int index) {
    for (int i = 0; i < 5; ++i) m_highlights[i]->setVisible(i == index);
}
bool gdui::RadialMenu::ccTouchBegan(CCTouch* t, CCEvent*) {
    m_pressed = hit(t);
    highlight(m_pressed);
    return m_pressed >= 0;
}
void gdui::RadialMenu::ccTouchMoved(CCTouch* t, CCEvent*) { highlight(hit(t) == m_pressed ? m_pressed : -1); }
void gdui::RadialMenu::ccTouchCancelled(CCTouch*, CCEvent*) { highlight(-1); m_pressed = -1; }
void gdui::RadialMenu::ccTouchEnded(CCTouch* t, CCEvent*) {
    int pressed = m_pressed;
    bool activate = pressed >= 0 && hit(t) == pressed;
    highlight(-1);
    m_pressed = -1;
    if (activate && m_original[pressed]->isEnabled()) m_original[pressed]->activate();
}
