#include "test_core_session.h"
#include <core_session.h>

void TestCoreSession::testSession() {
  //Tests for imageCut Struct
  {
    using ImageCut = core::ImageCut;

    {
      QSize q(10,100);
      ImageCut c; // default Constructor

      QCOMPARE(c.getWidth(q),   10u);
      QCOMPARE(c.getHeight(q), 100u);
      QCOMPARE((int)c.getCount(q), q.width()*q.height());
    }

    {
      uint top=20, bottom=60, left=5, right=5;
      QSize q(10,100);
      ImageCut c(top,bottom,left,right);

      QVERIFY(c == ImageCut(top,bottom,left,right));
      QCOMPARE(c.getWidth(q),   0u);
      QCOMPARE(c.getHeight(q), 20u);
    }
  }
}

// eof
