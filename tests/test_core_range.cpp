#include "test_core_range.h"

void TestCoreRange::testRange(){
  //Tests for Range
  //Basic Tests for default constructor
  {
    core::Range r;
    QVERIFY(!r.isValid());
  }

  //Basic Tests using one Argument constructor
  {
    qreal val1 = 5, val2 = 42;
    core::Range r(val1);
    QVERIFY(r.isValid());

    r.maximize();
    QVERIFY(r.isValid());

    r.set(val2);
    QVERIFY(r.contains(val2));
    QVERIFY(!r.contains(val1));

    r.extend(val1);
    QCOMPARE(r.min, val1);
    QCOMPARE(r.max, val2);

  }

  //Basic Tests using two Argument constructor
  {
    qreal min = 2, max = 42;
    core::Range r(min,max);
    QVERIFY(r.isValid());

    for(int i = min; i<max;++i){
      QVERIFY(r.contains(core::Range(min,i)));
    }

    for(int i = min; i<max;++i){
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

  //Tests for Ranges
  //default Constructor
  {
    int length = 3;
    core::Ranges R;
    QVERIFY(R.isEmpty());
    qreal min = 0, max=3;
    for(int i = 0; i<length;++i){
      R.add(core::Range(min,max));
      min = max +1;
      max = 2*max+1;
    }
    min =0; max=3;
    QVERIFY(!R.isEmpty());
    QCOMPARE(R.getData().size(),length);

    QCOMPARE(R.getData().at(0).min,(qreal)0);
    QCOMPARE(R.getData().at(1).min,(qreal)4);
    QCOMPARE(R.getData().at(2).min,(qreal)8);

    QVERIFY(R.rem(core::Range(min+1,max)));
    QCOMPARE(R.getData().at(0).min,(qreal)0);
    QCOMPARE(R.getData().at(0).max,(qreal)1);

    QVERIFY(!R.add(core::Range((qreal)0,(qreal)1)));
    QVERIFY(R.add(core::Range(min,max+1000)));
    QCOMPARE(R.getData().at(0).min,(qreal)0);
    QCOMPARE(R.getData().at(0).max,(qreal)max+1000);

  }

  //Tests for imageCut Struct
  {
    {
      QSize q(10,100);
      core::ImageCut c;//default Constructor
      QCOMPARE(c.getWidth(q),(uint)10);
      QCOMPARE(c.getHeight(q),(uint)100);
      QCOMPARE(c.getCount(q),(uint) q.width()*q.height());
    }

    {
      uint top=20,bottom=60,left=5,right=5;
      QSize q(10,100);
      core::ImageCut c(top,bottom,left,right);
      QVERIFY( c == core::ImageCut(top,bottom,left,right));
      QCOMPARE(c.getWidth(q),(uint)0);
      QCOMPARE(c.getHeight(q),(uint)20);
    }
  }
  //Tests for Borders struct
  {
    {
      qreal min = 0, max =10;
      core::Borders b;
      b.gamma.set(min,max);
      b.tth_gamma0.set(min,max);
      b.tth_regular.set(min,max);
      QVERIFY(b.isValid());
      b.invalidate();
      QVERIFY(!b.isValid());
    }
  }
  //Tests for TI_Data
  {
    {
      qreal tth = 1.2, inten=3.14;
      core::TI_Data t;
      // testing default Constructor, All data is NaN
      {
        QVERIFY(t.isEmpty());
        QVERIFY(t.getTth().isEmpty());
        QVERIFY(t.getInten().isEmpty());
        QVERIFY(qIsNaN(t.getTthRange().min));
        QVERIFY(qIsNaN(t.getTthRange().max));
        QVERIFY(qIsNaN(t.getIntenRange().min));
        QVERIFY(qIsNaN(t.getIntenRange().max));
      }
      {//testing if input of data is correct
        t.append(tth,inten);
        QCOMPARE(t.getTth().at(0),tth);
        QCOMPARE(t.getInten().at(0),inten);
        QCOMPARE(t.getTthRange().min,tth);
        QCOMPARE(t.getTthRange().max,tth);
        QCOMPARE(t.getIntenRange().min,inten);
        QCOMPARE(t.getIntenRange().max,inten);
        t.append(2*tth,0);
        QCOMPARE(t.getTth().at(1),2*tth);
        QCOMPARE(t.getInten().at(1),(qreal)0);
        QCOMPARE(t.getTthRange().min,(qreal)tth);
        QCOMPARE(t.getTthRange().max,(qreal)2*tth);
        QCOMPARE(t.getIntenRange().min,(qreal)0);
        QCOMPARE(t.getIntenRange().max,(qreal)inten);
        QVERIFY(t.isOrdered());
        t.clear();
        QVERIFY(t.isEmpty());
      }
    }
  }


}
