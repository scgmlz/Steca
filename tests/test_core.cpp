#include "test_core.h"
REGISTER_TEST_SUITE(TestCore)

void TestCore::testNaNs() {
  qreal r = qQNaN();
  float f1 = float(qQNaN()), f2 = float(r);

  QVERIFY(qIsNaN(r));
  QVERIFY(qIsNaN(f1));
  QVERIFY(qIsNaN(f2));
}

// eof
