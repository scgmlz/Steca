#ifndef TEST_APPROX_H
#define TEST_APPROX_H

#include <QtTest/QtTest>

#define TEST_SUITE

class TestApprox: public QObject {
  Q_OBJECT
bool nearlyEqual(qreal a, qreal b, qreal precision);
private slots:
  void testApproxFunction();
};


#endif
