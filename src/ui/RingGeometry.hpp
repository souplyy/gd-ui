#pragma once
#include <cmath>
#include <string>
#include <sstream>
namespace gdui {
constexpr double pi = 3.14159265358979323846;
// Index order: play, icons, stats, achievements, browse.
inline int ringSector(double x, double y, double inner, double outer) {
    auto r = std::hypot(x, y);
    if (r < inner || r > outer) return -1;
    double angle = std::atan2(y, x) * 180 / pi;
    double offset = std::fmod(angle - 54 + 360, 360);
    return static_cast<int>(offset / 72);
}
inline std::string ringSVG(int highlight = -1) {
    std::ostringstream s;
    s << "<svg xmlns='http://www.w3.org/2000/svg' width='240' height='240' viewBox='0 0 240 240'>";
    auto point = [](double r, double deg) {
        std::ostringstream p;
        p << 120 + r * std::cos(deg * pi / 180) << ',' << 120 - r * std::sin(deg * pi / 180);
        return p.str();
    };
    for (int i = 0; i < 5; ++i) {
        if (highlight >= 0 && highlight != i) continue;
        auto a = 54 + i * 72, b = a + 72;
        s << "<path d='M" << point(118, a) << " A118,118 0 0 0 " << point(118, b)
          << " L" << point(59, b) << " A59,59 0 0 1 " << point(59, a)
          << " Z' fill='" << (highlight >= 0 ? "#314556" : "#1c222b")
          << "' stroke='#465463' stroke-width='1'/>";
    }
    if (highlight < 0) s << "<circle cx='120' cy='120' r='53' fill='#16191f' stroke='#66798b' stroke-width='1'/>";
    s << "</svg>";
    return s.str();
}
}
