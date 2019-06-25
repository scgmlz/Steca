#define CATCH_CONFIG_MAIN
#include "core/typ/size2d.h"
#include "3rdparty/catch2/catch.hpp"

TEST_CASE( "Array2d - Size2d", "[04_array2d]" ) {
    size2d sz;
    CHECK((0 == sz.w && 0 == sz.h && sz.isEmpty() && 0 == sz.count()));

    size2d sz1(1, 2);
    CHECK((1 == sz1.w && 2 == sz1.h && !sz1.isEmpty() && 2 == sz1.count()));

    sz = sz1.transposed();
    CHECK((sz.w == sz1.h && sz.h == sz1.w));
}
