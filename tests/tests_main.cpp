#include "test_core_lib.h"
#include "test_core_array2d.h"
#include "test_core_image.h"
#include "test_core_range.h"

#define TEST_GROUP(TestClass)      \
{                                  \
  TestClass test;                  \
  QTest::qExec(&test, argc, argv); \
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  TEST_GROUP(TestCoreLib)
  TEST_GROUP(TestCoreArray2d)
  TEST_GROUP(TestCoreImage)
  TEST_GROUP(TestCoreRange)
}

// eof
