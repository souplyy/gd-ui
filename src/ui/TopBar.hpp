#pragma once

#include <Geode/Geode.hpp>

namespace gdui {
    class TopBar final : public cocos2d::CCLayerColor {
    public:
        static TopBar* create(MenuLayer* menuLayer);
    protected:
        bool init(MenuLayer* menuLayer);
        void refresh(float);
        bool apply(std::string const& content);
        void registerWithTouchDispatcher() override;
        bool ccTouchBegan(cocos2d::CCTouch*, cocos2d::CCEvent*) override;
        MenuLayer* m_owner = nullptr; // Parent owns this bar.
        cocos2d::CCMenu* m_controls = nullptr;
        std::string m_lastContent;
        std::string m_lastResolution;
        float m_height = 15.f;
    };
}
