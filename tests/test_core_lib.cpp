#include <QtTest/QtTest>
#include <core_lib.h>

class TestCoreLib: public QObject {
  Q_OBJECT

private slots:
  void conversions();
};

void TestCoreLib::conversions() {
  QCOMPARE(M_PI,core::rad_deg(180));
  QCOMPARE(90.0,core::deg_rad(M_PI_2));
  for_i(720)
    QCOMPARE((qreal)i,core::deg_rad(core::rad_deg(i)));
}

QTEST_MAIN(TestCoreLib)
#include "test_core_lib.moc"
