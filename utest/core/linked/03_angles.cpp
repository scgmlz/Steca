#define CATCH_CONFIG_MAIN
#include "core/base/angles.h"
#include <qmath.h>
#include "3rdparty/catch2/catch.hpp"

TEST_CASE( "Angles - Conversion", "[03_angles]" ) {
    CHECK(qreal(deg(rad(M_PI_2))) == 90);
    CHECK(qreal(rad(deg(90))) == M_PI_2);
}

TEST_CASE( "Angles - Normalization", "[03_angles]" ) {
    CHECK(qreal(deg(10)) == qreal(deg(730).normalized()));
    CHECK(qreal(deg(350)) == qreal(deg(-730).normalized()));
}
