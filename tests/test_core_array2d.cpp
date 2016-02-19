#include "test_core_array2d.h"

#include <core_array2d.h>

void TestCoreArray2d::testArray2d() {
  core::Array2D<qreal> a; // default constructor

  //Basic Test
  {
    auto aSize = a.getSize();
    QCOMPARE(aSize,QSize(0,0));
    QVERIFY(aSize.isValid() && aSize.isEmpty());
    QSize size(10,20);
    a.fill(size);
    QCOMPARE(size,a.getSize());
  }

  //Testing Methods for Data input and access
  {
    int const xSize = 10, ySize = 20;
    qreal val = 3;
    QSize size(xSize,ySize);
    a.fill(val,size);
    QCOMPARE(size,a.getSize());

    for (int x=0; x<xSize; ++x) {
      for (int y=0; y<ySize; ++y) {
        QCOMPARE(a.at(a.index(x,y)),val);
      }
    }

    for (int x=0; x<xSize; ++x) {
      for (int y=0; y<ySize; ++y) {
        a.setAt(a.index(x,y),x + y*xSize);
      }
    }

    for (int x=0; x<xSize; ++x) {
      for (int y=0; y<ySize; ++y) {
        QCOMPARE(a.at(a.index(x,y)),(qreal)(x + y*xSize));
      }
    }

    int const iSize = xSize * ySize;
    QCOMPARE(iSize,(int)a.getCount());

    for_i (iSize) {
      QCOMPARE(a[i],(qreal)i);
    }

    auto data = a.getData();
    for_i (iSize) {
      QCOMPARE(data[i],(qreal)i);
    }
  }

  //Testing if Array is reset to default after clear
  {
    a.clear();
    QCOMPARE(a.getSize(),QSize(0,0));
    QCOMPARE(a.getCount(),(uint)0);
  }
}

// eof
