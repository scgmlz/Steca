#include "test_save_load_json.h"
#include "core_fit_functions.h"
#include "core_types.h"
#include "thehub.h"

void TestSaveLoadJson::testSaveLoadJson() {
  qreal detdis = 1000;
  qreal pixsize = 1;
  bool  beamoff = false;
  qreal ox = 0;
  qreal oy = 0;
  uint cutVal = 5;
  core::ImageCut cut(cutVal,cutVal,cutVal,cutVal);
  int bgdegree = 2;

  // parameters ( range / value )
  qreal r0 = 0;
  qreal r1 = 42;
  qreal val1 = 1;

  QJsonObject pObj;
  core::fit::Function::Parameter p1;
  p1.setRange(r1,r1); // finite,finite
  p1.setValue(val1,5,true);
  p1.saveTo(pObj);
  core::fit::Function::Parameter p3;
  p3.loadFrom(pObj);
  QCOMPARE(p3.getRange().min, r1);
  QCOMPARE(p3.getRange().max, r1);
  QCOMPARE(p3.getValue(), val1);

  core::fit::Function::Parameter p2;
  p2.setValue(val1,5,true);
  p2.saveTo(pObj);
  core::fit::Function::Parameter p4;
  p4.loadFrom(pObj);
  QVERIFY(qIsInf(p4.getRange().min));
  QVERIFY(qIsInf(p4.getRange().max));
  QCOMPARE(p4.getValue(), val1);

  // functions

//  QTemporaryFile file;
//  file.open();
//  file.setFileName("tmpFileJSON");

//  TheHub testHub;
//  testHub.setGeometry(detdis,pixsize,beamoff,QPoint(ox,oy));
//  testHub.setImageCut(true,true,cut);

//  testHub.setBackgroundPolynomialDegree(bgdegree);

//  testHub.getBgRanges().add(core::Range(0,42));
//  file.write(testHub.save());

//  //file.close();
//  //file.open();

//  QFileInfo info(file);

//  testHub.load(info);

}

// eof
