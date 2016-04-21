#include "test_core_trees.h"
#include "types/core_type_trees.h"
#include "types/core_coords.h"

//------------------------------------------------------------------------------

using NormalizedAngle = core::NormalizedAngle<qreal>;

class PolePoint: public core::tree::RangeTreeItem<NormalizedAngle> {
  SUPER(PolePoint,core::tree::RangeTreeItem<NormalizedAngle>)
public:
  PolePoint(qreal beta);

  NormalizedAngle val() const { return beta_; }
  int compareToEqual(super::rcItem) const;

private:
  NormalizedAngle beta_;
};

PolePoint::PolePoint(qreal beta): beta_(beta) {
}

int PolePoint::compareToEqual(super::rcItem that) const {
  ASSERT(dynamic_cast<thisClass const*>(&that))
  ASSERT(beta_ == static_cast<thisClass const*>(&that)->beta_)

  if (this < &that) return -1;
  if (this > &that) return +1;
  return 0;
}

//------------------------------------------------------------------------------

using PolePointsTree = core::tree::RangeTree<PolePoint,NormalizedAngle>;

class PolePoints: public PolePointsTree {
  SUPER(PolePoints,PolePointsTree)
public:
  item_lst findAllBetweenAngles(qreal,qreal) const;
};

PolePoints::item_lst PolePoints::findAllBetweenAngles(qreal min, qreal max) const {
  ASSERT(min < max)
  ASSERT(-360 < min && max < 360)

  if (min < 0 && 0 < max)
    return super::findAllBetween(min,NormalizedAngle(360,true))
         + super::findAllBetween(0,max);

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
