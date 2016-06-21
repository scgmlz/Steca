#include "test_core_polefigure.h"
/* TODO
REGISTER_TEST_SUITE(TestCorePolefigure)


#include "core_polefigure.h"
#include "core_session.h"

namespace core {
  void calculateAlphaBeta(rcDataset  dataset,
                            deg tth, deg gamma,
                            deg& alpha, deg& beta);

  namespace pole {
  enum class Quadrant {
    NORTHEAST,
    SOUTHEAST,
    SOUTHWEST,
    NORTHWEST,
  };

  static int NUM_QUADRANTS = 4;
  typedef vec<Quadrant> Quadrants;
    deg angle(deg alpha1, deg alpha2, deg deltaBeta);

    bool inRadius(deg alpha, deg beta,
              deg centerAlpha, deg centerBeta,
              deg radius);

    template<typename Container>
    qreal inverseDistanceWeighing(Container const& distances,
                                  Container const& values);

    void inverseDistanceWeighing(qreal_vec const& distances,
                             vec<ReflectionInfo const*> const& infos,
                             ReflectionInfo& out);

    deg calculateDeltaBeta(deg beta1, deg beta2);

    bool inQuadrant(Quadrant quadrant, deg deltaAlpha, deg deltaBeta);

    Quadrant remapQuadrant(Quadrant);

    core::Range gammaRangeAt(core::shp_Lens lenses, qreal const tth);

    void searchInQuadrants(
      Quadrants const& quadrants,
      deg alpha, deg beta,
      deg searchRadius,
      ReflectionInfos const& infos,
      vec<ReflectionInfo const*> & foundInfos,
      qreal_vec & distances);

  }
}

#include "core_polefigure.h"

#include "test_core_lens.h"
#include "core_dataset.h"
#include "types/core_type_geometry.h"
#include "types/core_type_matrix.h"

#include <algorithm>
#include <QLinkedList>
#include <qmath.h>

using namespace core;
using namespace pole;

#define QALMOST_COMPARE(a,b) QVERIFY(qAbs(a-b) < 1E-6)
void TestCorePolefigure::testRelatedMethods() {

  {
    QCOMPARE(rad(M_PI),deg(180).toRad());
    QCOMPARE(deg(90.0),rad(M_PI_2).toDeg());

    for_i (720)
      QALMOST_COMPARE(deg(i),deg(i).toRad().toDeg());
  }

  {
    qreal phiDet = 0;
    qreal chiDet = 0;

    auto m1 = matrix3d ::rotationCWz(phiDet);
    matrix3d cmpMatrix(1, 0, 0,
                       0, 1, 0,
                       0, 0, 1);

    QCOMPARE(m1, cmpMatrix);

    auto m2 = core::matrix3d::rotationCWx(chiDet);
    QCOMPARE(m2, cmpMatrix);
  }

  {
    deg beta1 = 90;
    deg beta2 = 0;
    auto b = calculateDeltaBeta(beta1,beta2);
    QCOMPARE(b,beta1);
  }

  {
    deg angle1 = 90;
    deg angle2 = 90;
    deg angle3 = 0;
    auto a = angle(angle1,angle2,angle3);
    QCOMPARE(a,deg(0));
  }

  {
    deg alpha = 50;
    deg beta = 40;
    deg centerAlpha = 90;
    deg centerBeta = 0;
    deg radius = 90;

    QVERIFY(inRadius(alpha,beta,centerAlpha,centerBeta,radius));
  }

}

void TestCorePolefigure::testInQuadrant() {
  deg deltaAlpha;
  deg deltaBeta;

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

void TestCorePolefigure::testInverseDistanceWeighing() {
  qreal_vec distances;
  distances.append(2.0);
  distances.append(1.0);
  distances.append(1.0);
  distances.append(2.0);

  inten_t inten = 2;
  qreal tth     = 1;
  qreal fwhm    = 4;
  ReflectionInfos rinfos;
  for_i (4) {
    rinfos.append(ReflectionInfo(0,0,Range(),inten,tth,fwhm));
  }

  vec<ReflectionInfo const*> infos;
  for_i (4) {
    infos.append(&rinfos.at(i));
  }

  ReflectionInfo out;
  qreal tmp_height = 0;
  qreal tmp_offset = 0;
  qreal tmp_fwhm   = 0;
  inverseDistanceWeighing(distances,infos,out);
  for_i (NUM_QUADRANTS) {
    tmp_height += infos[0]->inten() * (qreal)1/distances[i];
    tmp_offset += infos[0]->tth()   * (qreal)1/distances[i];
    tmp_fwhm   += infos[0]->fwhm()  * (qreal)1/distances[i];
  }
  QCOMPARE(out.inten(),tmp_height/3);
  QCOMPARE(out.tth(),deg(tmp_offset/3));
  QCOMPARE(out.fwhm(),tmp_fwhm/3);
}

void TestCorePolefigure::testSearchInQuadrants() {
  qreal alpha = 15, beta = 20, searchRadius = 5;
  vec<ReflectionInfo const*> foundInfos;
  ReflectionInfos infos;
  ReflectionInfo in;
  // inside of BETA_LIMIT
  infos.append(ReflectionInfo(20,20,Range()));
  infos.append(ReflectionInfo(180,180,Range()));

  qreal_vec distances;

  searchInQuadrants({Quadrant::NORTHEAST, Quadrant::SOUTHEAST,
                     Quadrant::SOUTHWEST, Quadrant::NORTHWEST},alpha,beta,
    searchRadius,infos,foundInfos,distances);

  QCOMPARE(foundInfos.size(),4);

  QVERIFY(foundInfos.at(0));
  QCOMPARE(foundInfos.at(0)->alpha(),infos.at(0).alpha());

  QVERIFY(!foundInfos.at(1));
  QVERIFY(!foundInfos.at(2));
  QVERIFY(!foundInfos.at(3));
}

static core::Dataset testDataset(QSize size, core::inten_t inten, qreal_vec motorAngles, qreal mon, qreal deltaTime) {
  core::Metadata md;
  md.date = "15.03.2016";
  md.comment = "comment";
  md.motorXT = motorAngles[0];
  md.motorYT = motorAngles[1];
  md.motorZT = motorAngles[2];
  md.motorOmg = motorAngles[3];
  md.motorTth = motorAngles[4];
  md.motorPhi = motorAngles[5];
  md.motorChi = motorAngles[6];
  md.motorPST = motorAngles[7];
  md.motorSST = motorAngles[8];
  md.motorOMGM = motorAngles[9];

  vec<core::inten_t> intenVector;
  for_i (size.width() * size.height()) {
    intenVector.append(inten);
  }
  core::inten_t const* intensities = intenVector.constData();

  return core::Dataset (md, size, intensities);
}

void TestCorePolefigure::testCalcAlphaBeta() {
  deg alpha;
  deg beta;
  qreal_vec angles;
  qreal mon = 100, deltaTime = 8, tth = 0, gamma = 0;
  Session s;
  {
    angles.fill(0,10);
    auto const dataset = testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tth, gamma, alpha,beta);

    QCOMPARE(alpha,rad(acos(0)).toDeg());
    QCOMPARE(beta,rad(atan2(0,1)).toDeg());
  }

  {
    angles.fill(0,10);
    angles[3] = rad(M_PI/2).toDeg();

    auto const dataset = testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tth, gamma, alpha,beta);
    //rotated = -1,0,0
    QCOMPARE(alpha,rad(acos(0)).toDeg());
    QCOMPARE(beta,rad(atan2(-1,0) + 2 * M_PI).toDeg());
  }

  {
    angles.fill(0,10);
    angles[5] = rad(M_PI/2).toDeg();

    auto const dataset = testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tth, gamma, alpha,beta);
    // rotated = -1,0,0
    QCOMPARE(alpha,rad(acos(0)).toDeg());
    QCOMPARE(beta,rad(atan2(-1,0) + 2*M_PI).toDeg());
  }

  {
    angles.fill(0,10);
    angles[6] = rad(M_PI/2).toDeg();

    auto const dataset = testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tth, gamma, alpha,beta);
    // rotated = 0,0,1
    QCOMPARE(alpha,rad(acos(1)).toDeg());
    QCOMPARE(beta,rad(atan2(0,0)).toDeg());
  }

  {
    angles.fill(0,10);
    qreal tthRef = 90.0;
    auto const dataset = testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tthRef, gamma, alpha,beta);
    // rotated = 0,-1,0
    QCOMPARE(alpha,rad(acos(0)).toDeg());
    QCOMPARE(beta,rad(atan2(sin(M_PI/4),cos(M_PI/4))).toDeg());
  }

  {
    angles.fill(0,10);
    qreal gammaRef = 90;

    auto const dataset = testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tth, gammaRef, alpha,beta);
    // rotated = 0,0,1
    QCOMPARE(alpha,rad(acos(1)).toDeg());
    QCOMPARE(beta,rad(atan2(0,0)).toDeg());
  }

  {
    angles.fill(0,10);
    angles[0] = rad(M_PI).toDeg();
    angles[1] = rad(M_PI).toDeg();
    angles[2] = rad(M_PI).toDeg();

    qreal const tthRef = 4*rad(M_PI).toDeg();
    qreal const gammaRef = rad(M_PI).toDeg();

    auto const dataset = testDataset(QSize(10,10),42,angles,mon,deltaTime);
    calculateAlphaBeta(dataset,tthRef, gammaRef, alpha,beta);

    QCOMPARE(alpha,rad(acos(0)).toDeg());
  }
}
*/
// eof
