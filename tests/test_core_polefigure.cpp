#include "test_core_polefigure.h"
#include "core_polefigure.h"

#include "test_core_lens.h"
#include "core_dataset.h"

#include <algorithm>
#include <cmath>
#include <QLinkedList>
#include <QtMath>

using namespace core;

void TestCorePolefigure::testPolefigure() {

}

void TestCorePolefigure::testInQuadrant() {
  qreal deltaAlpha;
  qreal deltaBeta;

  {
    deltaAlpha = 0; deltaBeta = 0;

    QVERIFY(inQuadrant(Quadrant::NORTHEAST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::NORTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::SOUTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::SOUTHEAST,deltaAlpha,deltaBeta));
  }

  {
    deltaAlpha = -1; deltaBeta = 0;
    QVERIFY(!inQuadrant(Quadrant::NORTHEAST,deltaAlpha,deltaBeta));
    QVERIFY(inQuadrant(Quadrant::NORTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::SOUTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::SOUTHEAST,deltaAlpha,deltaBeta));
  }

  {
    deltaAlpha = -1; deltaBeta = -1;
    QVERIFY(!inQuadrant(Quadrant::NORTHEAST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::NORTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(inQuadrant(Quadrant::SOUTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::SOUTHEAST,deltaAlpha,deltaBeta));
  }

  {
    deltaAlpha = 0; deltaBeta = -1;
    QVERIFY(!inQuadrant(Quadrant::NORTHEAST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::NORTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(!inQuadrant(Quadrant::SOUTHWEST,deltaAlpha,deltaBeta));
    QVERIFY(inQuadrant(Quadrant::SOUTHEAST,deltaAlpha,deltaBeta));
  }

}

void TestCorePolefigure::testGamaRange() {

    int width             = 2;
    int height            = 3;
    intens_t inten        = 42.0f;
    intens_t specialInten = 150.0f;
    int posIntensArray    = 0;
    qreal gamma           = 6.7;
    qreal gammaSpecial    = 44.0;
    qreal tth             = 4.2;
    qreal tthSpecial      = 88.0;
    int anglePosX         = 1;
    int anglePosY         = 2;
    
  { // simple test
    TestCoreLens test;
    Dataset dataset = test.testDataset(width,height,inten,specialInten,posIntensArray);
    DiffractionAnglesMap angleMapArray = test.testAngleMapArray(gamma,gammaSpecial,
                                                    tth,tthSpecial,
                                                    width,height,
                                                    anglePosX,anglePosY);

    qreal tthNext   = 3.0;
    qreal gammaNext = 8.0;
    angleMapArray.setAt(1,0,DiffractionAngles(gammaNext,tthNext));
    auto lensSystem = makeLensSystem(dataset, angleMapArray);
    qreal testTth = 4.0;
    Range extendRange = Range(gammaNext);
    auto range = gammaRangeAt(lensSystem,testTth);

    QCOMPARE(range.max, extendRange.max);
    QCOMPARE(range.min, extendRange.min);
  }

  { // extensiv test with full test data
    int width = 11;
    int height = 11;
    qreal fillVal = 0;
    int posVal = 0;
    TestCoreLens test;
    Dataset dataset = test.testDataset(width,height,fillVal,fillVal,posVal);
    DiffractionAnglesMap angleMapArray = test.testAngleMapArray(fillVal,fillVal,
                                                    fillVal,fillVal,
                                                    width,height,
                                                    posVal,posVal);
    qreal gamma = 5.0;
    for (int y = 0; y < height; ++y) {
      if (y > 0 && y <= 5) gamma = gamma - 1;
      else ++gamma;
      for (int x = 0; x < width; ++x) {
        angleMapArray.setAt(x,y,DiffractionAngles(gamma,(x+1)*(y+1)));
        angleMapArray.setAt(x,y,DiffractionAngles(gamma,(x+1)*(y+1)));
      }
    }
    auto lensSystem = makeLensSystem(dataset,angleMapArray);
    qreal testTth = 42;
    // min = gamma val minus 5  max = gamma val of last row
    Range extendRange = Range(gamma-5,gamma);
    auto range = gammaRangeAt(lensSystem,testTth);

    QCOMPARE(range.max, extendRange.max);
    QCOMPARE(range.min, extendRange.min);
  }

}

void TestCorePolefigure::testInverseDistanceWeighing() {
  TestContainer distances;
  distances.append(1.0);
  distances.append(2.0);
  distances.append(3.0);
  distances.append(4.0);

  TestContainer values;
  values.append(1.0);
  values.append(2.0);
  values.append(3.0);
  values.append(4.0);

  qreal in = inverseDistanceWeighing(distances,values);
  qreal cmp = (qreal)4.0/(qreal)(25.0/12.0);
  QCOMPARE(in,cmp);

}
