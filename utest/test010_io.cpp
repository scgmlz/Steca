#include "gtest/gtest.h"
#include "session.h"
#include "io/io_io.h"
#include "testdata.h"
#include "typ/exception.h"

class Session* gSession = Session::instance();

TEST(IO, Caress) {
    try {
        io::loadDatafile(TESTDATADIR "/caress.dat");
    } catch (Exception& ex) {
        std::cerr << "io::load throws: " << ex.what() << "\n";
        EXPECT_TRUE(false);
    } catch (...) {
        std::cerr << "io::load throws exception of unexpected type\n";
        EXPECT_TRUE(false);
    }
}
