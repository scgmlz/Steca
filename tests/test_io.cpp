#include "io_io.h"
#include "test/tests.h"

#ifdef TESTS

#include <QDir>
#include <QFileInfo>

str thisDir() {
  return QFileInfo(__FILE__).dir().absolutePath();
}

#endif

TEST("XY()",
     ({ CHECK_NOTHROW(io::loadCaress2(thisDir() + "/test_caress.dat")); });)
