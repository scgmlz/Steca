#include "core/base/angles.h"
#include <qmath.h>
#include "3rdparty/catch2/catch.hpp"

TEST_CASE( "Angles - Conversion", "" ) {
    CHECK(qreal(deg(rad(M_PI_2))) == 90);
    CHECK(qreal(rad(deg(90))) == M_PI_2);
}

TEST_CASE( "Angles - Normalization", "" ) {
    CHECK(qreal(deg(10)) == qreal(deg(730).normalized()));
    CHECK(qreal(deg(350)) == qreal(deg(-730).normalized()));
}
