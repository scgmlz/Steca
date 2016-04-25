#include "test_core_lens.h"

#include "core_dataset.h"
#include "types/core_type_geometry.h"
#include "types/core_type_image_transform.h"
#include "test_helpers.h"
#include "types/core_types_fwd.h"
#include "types/core_coords.h"
#include "core_session.h"
#include "thehub.h"
#include "types/core_type_curve.h"

#define TEST_DATA \
  int width             = 2;\
  int height            = 3;\
  inten_t inten        = 42.0f;\
  int mon               = 10;\
  int deltaTime         = 5; \
  
  
#define TEST_LENS \
 Session session; \
  QVector<qreal> motorAngles;\
  IJ midPix(1,1);\
  qreal midTTh = 2;\
  rcstr date = "15.03.2016";\
  rcstr comment = "comment";\
  qreal motorAngle = 0;\
  QVector<core::inten_t> intenVector;\
  for_i (width * height) {\
    intenVector.append(inten/2);\
  }\
  intenVector.prepend(inten);\
  core::inten_t const* intensities = intenVector.constData();\
  QSize size(width,height);\
  core::inten_t intensCorr = 10.0f;\
  \
  Datasets datasets;\
  shp_Dataset dataset(new Dataset(\
                       date,comment,\
                       motorAngle,motorAngle,motorAngle,\
                       motorAngle,motorAngle,motorAngle,motorAngle,\
                       motorAngle,motorAngle,motorAngle,\
                       mon, deltaTime,\
                       QSize(width,height), intensities));\
  datasets.appendHere(dataset);\
  AngleMap angleMap;\
  angleMap.calculate(midTTh,Geometry(),QSize(width,height),ImageCut(),midPix);\
  session.setImageSize(QSize(width,height));\
  Image corr(size);\
  for_i(size.width()*size.height()) corr.setAt(i,intensCorr);\
  \
  Lens lensNoTrans(session,*(dataset.data()),&corr,true,true,core::eNorm::NONE,\
  angleMap,ImageCut(),ImageTransform::ROTATE_0); \
  int mon2 = 80;\
  int deltaTime2 = 25;\
  QVector<core::inten_t> intenVector2;\
  for_i (width * height) {\
    intenVector2.append(inten*2);\
  }\
  auto intensities2 = intenVector2.data();\
  shp_Dataset dataset2(new Dataset(\
                     date,comment,\
                     motorAngle,motorAngle,motorAngle,\
                     motorAngle,motorAngle,motorAngle,motorAngle,\
                     motorAngle,motorAngle,motorAngle,\
                     mon2, deltaTime2,\
                     QSize(width,height), intensities2));\
  datasets.appendHere(dataset2);

using namespace core;

void TestCoreLens::testNormLens() {
  TEST_DATA
  TEST_LENS

  {
    {
      Lens lensNorm(session,*(dataset.data()),&corr,true,true,
      core::eNorm::DELTA_MONITOR_COUNT,
      angleMap,ImageCut(),ImageTransform::ROTATE_0);
      QCOMPARE(lensNorm.inten(0,0),inten*4.5);
    }
    
    {
      Lens lensNorm(session,*(dataset.data()),&corr,true,true,core::eNorm::DELTA_TIME,
      angleMap,ImageCut(),ImageTransform::ROTATE_0);
      QCOMPARE(lensNorm.inten(0,0),inten*((deltaTime2+deltaTime)*0.5/deltaTime));
    }
  }
  
}

void TestCoreLens::testTransformationLens() {
  TEST_DATA
  TEST_LENS
  { 
    Lens lensTrans(session,*(dataset.data()),&corr,true,true,core::eNorm::NONE,
    angleMap,ImageCut(),ImageTransform::ROTATE_1);
    QCOMPARE(lensNoTrans.size().width(),lensTrans.size().height());
    QCOMPARE(lensNoTrans.size().height(),lensTrans.size().width());
    QCOMPARE(lensNoTrans.inten(0,0),lensTrans.inten(2,0));  
  }
  
  { 
    Lens lensTrans(session,*(dataset.data()),&corr,true,true,core::eNorm::NONE,
    angleMap,ImageCut(),ImageTransform::ROTATE_2);
    QCOMPARE(lensNoTrans.size().width(),lensTrans.size().width());
    QCOMPARE(lensNoTrans.size().height(),lensTrans.size().height());
    QCOMPARE(lensNoTrans.inten(0,0),lensTrans.inten(1,2));  
  }

  { 
    Lens lensTrans(session,*(dataset.data()),&corr,true,true,core::eNorm::NONE,
    angleMap,ImageCut(),ImageTransform::ROTATE_3);
    QCOMPARE(lensNoTrans.size().width(),lensTrans.size().height());
    QCOMPARE(lensNoTrans.size().height(),lensTrans.size().width());
    QCOMPARE(lensNoTrans.inten(0,0),lensTrans.inten(0,1));  
  }
  
  { 
    Lens lensTrans(session,*(dataset.data()),&corr,true,true,core::eNorm::NONE,
    angleMap,ImageCut(),ImageTransform::MIRROR_ROTATE_0);
    QCOMPARE(lensNoTrans.size().width(),lensTrans.size().width());
    QCOMPARE(lensNoTrans.size().height(),lensTrans.size().height());
    QCOMPARE(lensNoTrans.inten(0,0),lensTrans.inten(1,0));  
  }

  { 
    Lens lensTrans(session,*(dataset.data()),&corr,true,true,core::eNorm::NONE,
    angleMap,ImageCut(),ImageTransform::MIRROR_ROTATE_1);
    QCOMPARE(lensNoTrans.size().width(),lensTrans.size().height());
    QCOMPARE(lensNoTrans.size().height(),lensTrans.size().width());
    QCOMPARE(lensNoTrans.inten(0,0),lensTrans.inten(2,1));  
  }

  { 
    Lens lensTrans(session,*(dataset.data()),&corr,true,true,core::eNorm::NONE,
    angleMap,ImageCut(),ImageTransform::MIRROR_ROTATE_2);
    QCOMPARE(lensNoTrans.size().width(),lensTrans.size().width());
    QCOMPARE(lensNoTrans.size().height(),lensTrans.size().height());
    QCOMPARE(lensNoTrans.inten(0,0),lensTrans.inten(0,2));  
  }
  
  { 
    Lens lensTrans(session,*(dataset.data()),&corr,true,true,core::eNorm::NONE,
    angleMap,ImageCut(),ImageTransform::MIRROR_ROTATE_3);
    QCOMPARE(lensNoTrans.size().width(),lensTrans.size().height());
    QCOMPARE(lensNoTrans.size().height(),lensTrans.size().width());
    QCOMPARE(lensNoTrans.inten(0,0),lensTrans.inten(0,0));
  } 
}

void TestCoreLens::testCutLens() {
  TEST_DATA
  TEST_LENS
  {
    Lens lensCut(session,*(dataset.data()),&corr,true,true,core::eNorm::NONE,
    angleMap,ImageCut(0,0,1,2),ImageTransform::ROTATE_0);
    QCOMPARE(lensCut.inten(0,0),inten);
    QCOMPARE(lensCut.size(),QSize(1,1));
  }
}

void TestCoreLens::testSensitivityCorrectionLens() {
  TEST_DATA
  TEST_LENS
  {
    Lens lensCorr(session,*(dataset.data()),&corr,true,true,core::eNorm::NONE,
    angleMap,ImageCut(),ImageTransform::ROTATE_0);
    lensCorr.intensCorr_.setAt(0,1,4);
    QCOMPARE(lensCorr.inten(0,1),2*inten);
  }
}

void TestCoreLens::testIntensityRangeLens() {
  TEST_DATA
  TEST_LENS
  
  {
    QCOMPARE(lensNoTrans.rgeInten().max,inten);
    QCOMPARE(lensNoTrans.rgeInten().min,inten/2);    
    
    Lens lensCut(session,*(dataset.data()),&corr,true,true,core::eNorm::NONE,
    angleMap,ImageCut(1,0,0,2),ImageTransform::ROTATE_0);
    QCOMPARE(lensCut.rgeInten().max,inten/2);
    QCOMPARE(lensCut.rgeInten().min,inten/2);
  }
  
  {
    Lens lensCut(session,*(dataset.data()),&corr,true,true,core::eNorm::NONE,
    angleMap,ImageCut(1,0,0,0),ImageTransform::ROTATE_0);
    QCOMPARE(lensCut.angles(0,0).gamma,lensNoTrans.angles(1,0).gamma);
    QCOMPARE(lensCut.angles(0,0).tth,lensNoTrans.angles(1,0).tth);  
  }
}

void TestCoreLens::testGlobalIntensityRangeLens() {
  TEST_DATA
  TEST_LENS
  {
    Lens lensGlobRange(session,*(dataset.data()),&corr,true,true,core::eNorm::NONE,
    angleMap,ImageCut(0,0,0,0),ImageTransform::ROTATE_0);
    QCOMPARE(lensGlobRange.rgeIntenGlobal().max,(double)84);
    QCOMPARE(lensGlobRange.rgeIntenGlobal().min,(double)21);
  }
}


