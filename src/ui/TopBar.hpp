#pragma once

#include <Geode/Geode.hpp>

namespace gdui {
    class TopBar final : public cocos2d::CCLayerColor {
    public:
        static TopBar* create(MenuLayer* menuLayer);

    protected:
        bool init(MenuLayer* menuLayer);
        void registerWithTouchDispatcher() override;
        bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    };
}
