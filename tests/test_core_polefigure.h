#ifndef TEST_CORE_POLEFIGURE_H
#define TEST_CORE_POLEFIGURE_H

#include "core_polefigure.h"

#include <algorithm>
#include <cmath>
#include <Eigen/Core>
#include <QLinkedList>
#include <QtMath>

#include <QtTest/QtTest>

class TestPolefigure : public QObject {
  Q_OBJECT



private slots:
  void testPolefigure();
  void testRotation();
  void testCalcAlphaBeta();
  void testInQuadrant();
  void testRemapQuadrant();
  void testGamaRange();
  void inverseDistanceWeighing();

};

#endif // TEST_CORE_POLEFIGURE_H
