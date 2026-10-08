#include <Geode/Geode.hpp>
#include <Geode/ui/SceneEvent.hpp>
#include "ui/UIToggle.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    SceneEvent().listen([](cocos2d::CCScene* scene) {
        gdui::addUIToggle(scene);
        return false;
    }).leak();
    log::info("GD UI loaded (enabled: {})", Mod::get()->getSettingValue<bool>("enabled"));
}
