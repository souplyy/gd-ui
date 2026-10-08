#pragma once
#include <Geode/Geode.hpp>
#include <array>
namespace gdui {
class RadialMenu final : public cocos2d::CCLayer {
public:
    static RadialMenu* create(MenuLayer* owner);
    void setMode(bool enabled);
    void onEnter() override;
protected:
    bool init(MenuLayer* owner);
    void registerWithTouchDispatcher() override;
    bool ccTouchBegan(cocos2d::CCTouch*, cocos2d::CCEvent*) override;
    void ccTouchMoved(cocos2d::CCTouch*, cocos2d::CCEvent*) override;
    void ccTouchEnded(cocos2d::CCTouch*, cocos2d::CCEvent*) override;
    void ccTouchCancelled(cocos2d::CCTouch*, cocos2d::CCEvent*) override;
private:
    int hit(cocos2d::CCTouch*);
    void highlight(int index);
    std::array<geode::Ref<cocos2d::CCMenuItem>, 5> m_original;
    std::array<bool, 5> m_originalVisible{};
    std::array<cocos2d::CCSprite*, 5> m_highlights{};
    SimplePlayer* m_player = nullptr;
    float m_radius = 108;
    int m_pressed = -1;
};
}
