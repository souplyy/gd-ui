#pragma once
#include <Geode/Geode.hpp>
namespace gdui {
class ProfileMenu final : public cocos2d::CCLayer {
public:
    static ProfileMenu* create(MenuLayer* owner, float width, float height);
    bool isBusy() const;
protected:
    bool init(MenuLayer*, float, float);
    void registerWithTouchDispatcher() override;
    bool ccTouchBegan(cocos2d::CCTouch*, cocos2d::CCEvent*) override;
private:
    void toggle(cocos2d::CCObject*);
    void profile(cocos2d::CCObject*);
    void icons(cocos2d::CCObject*);
    void account(cocos2d::CCObject*);
    void close();
    void finishClose();
    MenuLayer* m_owner = nullptr;
    cocos2d::CCNode* m_panel = nullptr;
    cocos2d::CCMenu* m_triggerControls = nullptr;
    cocos2d::CCMenu* m_panelControls = nullptr;
    cocos2d::CCPoint m_panelHome;
    bool m_animating = false;
    bool m_open = false;
};
}
