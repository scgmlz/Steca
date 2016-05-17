#include "test_core_trees.h"
REGISTER_TEST_SUITE(TestCoreTrees)

#include "types/core_angles.h"
#include "types/core_type_trees.h"

//------------------------------------------------------------------------------

using deg = core::deg;
using NormalizedDeg = core::NormalizedDeg;

class PolePoint: public core::tree::RangeTreeItem<NormalizedDeg> {
  SUPER(PolePoint,core::tree::RangeTreeItem<NormalizedDeg>)
public:
  PolePoint(qreal beta);

  NormalizedDeg val() const { return beta_; }
  int compareToEqual(super::rcItem) const;

private:
  NormalizedDeg beta_;
};

PolePoint::PolePoint(qreal beta): beta_(beta) {
}

int PolePoint::compareToEqual(super::rcItem that) const {
  EXPECT(dynamic_cast<thisClass const*>(&that))
  EXPECT(beta_ == static_cast<thisClass const*>(&that)->beta_)

  if (this < &that) return -1;
  if (this > &that) return +1;
  return 0;
}

//------------------------------------------------------------------------------

using PolePointsTree = core::tree::RangeTree<PolePoint,NormalizedDeg>;

class PolePoints: public PolePointsTree {
  SUPER(PolePoints,PolePointsTree)
public:
  item_lst findAllBetweenAngles(deg,deg) const;
};

PolePoints::item_lst PolePoints::findAllBetweenAngles(deg min, deg max) const {
  EXPECT(min < max && -360 < min && max < 360)

  if (min < 0 && 0 < max)
    return super::findAllBetween(NormalizedDeg(min),NormalizedDeg(360,true))
         + super::findAllBetween(NormalizedDeg(0),NormalizedDeg(max));

  return super::findAllBetween(min,max);
}

//------------------------------------------------------------------------------

void TestCoreTrees::testIntervalTree() {
  qreal minAngle(-10), maxAngle(+10); // range <min,max)

  QList<PolePoint> ps;
  uint cnt = 10000, cntInRange = 0;

  // make a random collection
  for_i (cnt) {
    qreal rnd = 270*((qreal)random()/RAND_MAX - .5);
    if (minAngle <= rnd && rnd < maxAngle)
      ++cntInRange;

    ps.append(PolePoint(rnd));
  }

  TR(cnt << "points total," << cntInRange << "in range")

  // insert into an interval tree
  PolePoints pps;
  for (auto &p: ps)
    pps.insert(p);

  // check that all can be found
  for (auto &p: ps)
    QVERIFY(pps.find(p));

  // get all in range
  auto psFound = pps.findAllBetweenAngles(minAngle,maxAngle);
  QCOMPARE((int)cntInRange,psFound.count());
}

// eof
