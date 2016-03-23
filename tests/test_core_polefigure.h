#ifndef TEST_CORE_POLEFIGURE_H
#define TEST_CORE_POLEFIGURE_H

#include "core_polefigure.h"

#include <algorithm>
#include <cmath>
#include <Eigen/Core>
#include <QLinkedList>
#include <QtMath>

#include <QtTest/QtTest>

//struct TestContainer {
//  qreal data1, data2, data3, data4;

//  int size() const { return 4; }


//};

typedef QVector<qreal> TestContainer;

template<TestContainer *container> qreal inverseDistanceWeighing(TestContainer const& distances,
                                             TestContainer const& values);


class TestPolefigure : public QObject {
  Q_OBJECT

private slots:
  void testPolefigure();
  void testInQuadrant();
  void testGamaRange();
  void testInverseDistanceWeighing();

};

#endif // TEST_CORE_POLEFIGURE_H
