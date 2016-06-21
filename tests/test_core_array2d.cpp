#include "test_core_array2d.h"
REGISTER_TEST_SUITE(TestCoreArray2d)

#include "types/core_type_array2d.h"

void TestCoreArray2d::testArray2d() {
  core::Array2D<qreal> a; // default constructor

  { // basic test
    auto aSize = a.size();
    QCOMPARE(aSize, core::size2d(0,0));
    QVERIFY(aSize.isEmpty());

    core::size2d size(10,20);
    a.fill(size);
    QCOMPARE(size,a.size());
  }

  { // methods for data input and access
    uint const iSize = 10, jSize = 20;
    qreal val = 3;

    core::size2d size(iSize,jSize);
    a.fill(val,size);
    QCOMPARE(size,a.size());

    for_ij (iSize,jSize)
      QCOMPARE(a.at(a.index(i,j)), val);

    for_ij (iSize,jSize)
      a.setAt(a.index(i,j), i + j*iSize);

    for_ij (iSize,jSize)
      QCOMPARE(a.at(a.index(i,j)),qreal(i + j*iSize));

    uint const count = iSize * jSize;
    QCOMPARE(count, a.count());

    for_i (count) {
      QCOMPARE(a[i], qreal(i));
    }

    auto data = a.data();
    for_i (count)
      QCOMPARE(data[i], qreal(i));
  }

  { // is Array2D reset to default after clear?
    a.clear();
    QCOMPARE(a.size(),  core::size2d(0,0));
    QCOMPARE(a.count(), 0u);
  }
}

// eof
