#pragma once

#include <Geode/ui/Popup.hpp>

namespace gdui {
    class StarterPopup final : public geode::Popup {
    protected:
        bool init();

    public:
        static StarterPopup* create();
    };
}
