#include "core/fitengine/double_with_error.h"
#include "3rdparty/catch2/catch.hpp"

TEST_CASE( "Rounding - FitParError", "" ) {
    DoubleWithError A{33., 5127.1234};
    CHECK(5127 == A.roundedError(4));
    CHECK(5130 == A.roundedError(3));
    CHECK(5100 == A.roundedError(2));
    CHECK(5000 == A.roundedError(1));

    DoubleWithError B{33., 27.1234567};
    CHECK(27.12 == B.roundedError(4));
    CHECK(27.1 == B.roundedError(3));
    CHECK(30. == B.roundedError(1));

    DoubleWithError C{33., 7.8765432};
    CHECK(7.88 == C.roundedError(4));
    CHECK(7.9 == C.roundedError(3));
    CHECK(8. == C.roundedError(2));
    CHECK(10. == C.roundedError(1));

    DoubleWithError D{33.e-1, 7.8765432e-1};
    CHECK(7.88e-1 == D.roundedError(4));
    CHECK(7.9e-1 == D.roundedError(3));
    CHECK(8.e-1 == D.roundedError(2));
    CHECK(10.e-1 == D.roundedError(1));

    DoubleWithError E{33.e-2, 7.8765432e-2};
    CHECK(7.88e-2 == E.roundedError(4));
    CHECK(7.9e-2 == E.roundedError(3));
    CHECK(8.e-2 == E.roundedError(2));
    CHECK(10.e-2 == E.roundedError(1));

    DoubleWithError F{33.e-3, 7.8765432e-3};
    CHECK(7.88e-3 == F.roundedError(4));
    CHECK(7.9e-3 == F.roundedError(3));
    CHECK(8.e-3 == F.roundedError(2));
    CHECK(10.e-3 == F.roundedError(1));
}
