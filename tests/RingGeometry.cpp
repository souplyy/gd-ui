#include "src/ui/RingGeometry.hpp"
#include <cassert>
int main() {
    using namespace gdui;
    for (int i=0; i<5; ++i) {
        double a=(90+72*i)*pi/180;
        assert(ringSector(80*cos(a),80*sin(a),59,118)==i);
    }
    assert(ringSector(0,0,59,118)==-1);
    assert(ringSector(200,0,59,118)==-1);
    int counts[5] = {};
    for (int a=0; a<360; ++a) {
        int sector = ringSector(80*cos((a+.5)*pi/180),80*sin((a+.5)*pi/180),59,118);
        assert(sector >= 0 && sector < 5);
        ++counts[sector];
    }
    for (auto count : counts) assert(count == 72);
}
