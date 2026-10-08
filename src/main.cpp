#include <Geode/Geode.hpp>

using namespace geode::prelude;

$on_mod(Loaded) {
    log::info("GD UI loaded (enabled: {})", Mod::get()->getSettingValue<bool>("enabled"));
}
