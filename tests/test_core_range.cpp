#include "test_core_range.h"
#include <core_curve.h>
#include "core_types.h"

void TestCoreRange::testRange() {
  // Tests for Range
  // Basic Tests for default constructor
  {
    core::Range r;
    QVERIFY(!r.isValid());
  }

  // Basic Tests using one Argument constructor
  {
    qreal val1 = 5, val2 = 42;
    core::Range r(val1);
    QVERIFY(r.isValid());

    r = core::Range::infinite();
    QVERIFY(r.isValid());

    r.set(val2);
    QVERIFY(r.contains(val2));
    QVERIFY(!r.contains(val1));

    r.extend(val1);
    QCOMPARE(r.min, val1);
    QCOMPARE(r.max, val2);
  }

  // Basic Tests using two Argument constructor
  {
    qreal min = 2, max = 42;
    core::Range r(min,max);
    QVERIFY(r.isValid());

    for (int i = min; i<max; ++i) {
      QVERIFY(r.contains(core::Range(min,i)));
    }

    for (int i = min; i<max; ++i) {
      QVERIFY(r.contains(core::Range(i,max)));
    }

    r.safeSet(min,max);
    QCOMPARE(r.min,min);
    QCOMPARE(r.max,max);

    r.safeSet(max,min);
    QCOMPARE(r.min,min);
    QCOMPARE(r.max,max);

    QCOMPARE(r.safeFrom(max,min).min,core::Range(min,max).min);
    QCOMPARE(r.safeFrom(max,min).max,core::Range(min,max).max);

    r.extend(core::Range(min-max,max+min));
    QVERIFY(r.contains(core::Range(-40,44)));
    QVERIFY(r.intersects(core::Range(min,max+100)));
  }

  // Tests for Ranges
  // default Constructor
  {
    uint length = 3;
    core::Ranges rs;
    QVERIFY(rs.isEmpty());

    qreal min = 0, max = 3;
    for (uint i = 0; i<length; ++i) {
      rs.add(core::Range(min,max));
      min = max + 1;
      max = 2*max + 1;
    }

    min = 0; max= 3;
    QVERIFY(!rs.isEmpty());
    QCOMPARE(rs.count(),length);

    QCOMPARE(rs.at(0).min, 0.);
    QCOMPARE(rs.at(1).min, 4.);
    QCOMPARE(rs.at(2).min, 8.);

    QVERIFY(rs.rem(core::Range(min+1,max)));
    QCOMPARE(rs.at(0).min, 0.);
    QCOMPARE(rs.at(0).max, 1.);

    QVERIFY(!rs.add(core::Range(0., 1.)));
    QVERIFY(rs.add(core::Range(min,  max+1000.)));
    QCOMPARE(rs.at(0).min, 0.);
    QCOMPARE(rs.at(0).max, max+1000.);
  }

  //Tests for Curve
  {
    {
      qreal tth = 1.2, inten = 3.14;
      core::Curve t;

      // testing default Constructor, All data is NaN
      {
        QVERIFY(t.isEmpty());
        QVERIFY(t.getXs().isEmpty());
        QVERIFY(t.getYs().isEmpty());
        QVERIFY(qIsNaN(t.XRange().min));
        QVERIFY(qIsNaN(t.XRange().max));
        QVERIFY(qIsNaN(t.YRange().min));
        QVERIFY(qIsNaN(t.YRange().max));
      }

      //testing if input of data is correct
      {
        t.append(tth,inten);
        QCOMPARE(t.getXs().at(0),tth);
        QCOMPARE(t.getYs().at(0),inten);
        QCOMPARE(t.XRange().min,tth);
        QCOMPARE(t.XRange().max,tth);
        QCOMPARE(t.YRange().min,inten);
        QCOMPARE(t.YRange().max,inten);

        t.append(2*tth,0);
        QCOMPARE(t.getXs().at(1),   2*tth);
        QCOMPARE(t.getYs().at(1),   0.);
        QCOMPARE(t.XRange().min, tth);
        QCOMPARE(t.XRange().max, 2*tth);
        QCOMPARE(t.YRange().min, 0.);
        QCOMPARE(t.YRange().max, inten);
        QVERIFY(t.isOrdered());

        t.clear();
        QVERIFY(t.isEmpty());
      }
    }
  }
}

// eof
