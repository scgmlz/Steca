#include "test_save_load_json.h"
#include "core_fit_functions.h"
#include "core_types.h"

// make non-public methods public
#define TEST_FIT_CLASS(cls)                 \
  class Test##cls: public core::fit::cls {  \
public:                                     \
  void setValue(uint parIndex, qreal val) { \
    core::fit::cls::setValue(parIndex,val); \
  }                                         \
};

TEST_FIT_CLASS(Polynomial)
TEST_FIT_CLASS(Gaussian)
TEST_FIT_CLASS(CauchyLorentz)
TEST_FIT_CLASS(PseudoVoigt1)

void TestSaveLoadJson::testSaveLoadJson() {
  int error = 5;
  qreal r1 = 42;
  qreal val0 = 0;
  qreal val1 = 1;
  qreal val2 = 2;
  qreal val3 = 3;
  qreal val4 = 4;

  {
    core::JsonObj pObj;
    core::fit::Function::Parameter p1;
    p1.setRange(r1,r1); // finite,finite
    p1.setValue(val1,error,true);
    p1.saveTo(pObj);
    core::fit::Function::Parameter p3;
    p3.loadFrom(pObj);
    QCOMPARE(p3.getRange().min, r1);
    QCOMPARE(p3.getRange().max, r1);
    QCOMPARE(p3.getValue(), val1);

    core::fit::Function::Parameter p2;
    p2.setValue(val1,error,true);
    p2.saveTo(pObj);
    core::fit::Function::Parameter p4;
    p4.loadFrom(pObj);
    QVERIFY(qIsInf(p4.getRange().min));
    QVERIFY(qIsInf(p4.getRange().max));
    QCOMPARE(p4.getValue(), val1);
  }

  { // testing saveTo/loadFrom Polynomial
    TestPolynomial polyLoad;
    TestPolynomial polySave;

    polySave.setDegree(3);
    polySave.setParameterCount(4);
    polySave.setValue(0,val1);
    polySave.setValue(1,val2);
    polySave.setValue(2,val3);
    polySave.setValue(3,val4);

    core::JsonObj polyObj;

    polySave.saveTo(polyObj);

    QCOMPARE(polyLoad.getDegree(),0u);
    QCOMPARE(polyLoad.parameterCount(),1u);
    QCOMPARE(polyLoad.getParameter(0).getValue(),val0);

    polyLoad.loadFrom(polyObj);

    QCOMPARE(polyLoad.getDegree(),3u);
    QCOMPARE(polyLoad.parameterCount(),4u);
    QCOMPARE(polyLoad.getParameter(0).getValue(),val1);


    // testing saveTo/loadFrom CauchyLorentz
    TestCauchyLorentz cauchySave, cauchyLoad;
    core::JsonObj cObj;
    cauchySave.setValue(0,val1);
    cauchySave.setValue(1,val2);

    cauchySave.saveTo(cObj);
    cauchyLoad.loadFrom(cObj);

    QCOMPARE(cauchyLoad.parameterCount(),3u);
    QCOMPARE(cauchyLoad.getParameter(0).getValue(),val1);

    TestPseudoVoigt1 pseudoSave, pseudoLoad;
    core::JsonObj psObj;
    pseudoSave.setValue(0,val4);

    pseudoSave.saveTo(psObj);
    pseudoLoad.loadFrom(psObj);

    QCOMPARE(pseudoLoad.parameterCount(),4u);
    QCOMPARE(pseudoLoad.getParameter(0).getValue(),val4);
    bool verify = false;
    auto max = pseudoLoad.getParameter(0).getRange().max;
    if ( qIsInf(max))
      verify = (max > 0) ? true : false;

    QVERIFY(verify);

    // testing saveTo/loadFrom Gaussian

    TestGaussian gSave, gLoad;
    core::JsonObj gObj;
    gSave.setValue(0,val3);

    // Testing saveTo/loadFrom SumFunctions

    core::JsonObj sumObj;

    core::fit::SumFunctions sumSave, sumLoad;
    TestPolynomial *p1 = new TestPolynomial, *p2 = new TestPolynomial;

    p1->loadFrom(polyObj);
    p2->loadFrom(polyObj);

    sumSave.addFunction(p1);
    sumSave.addFunction(p2);

    sumSave.saveTo(sumObj);
    sumLoad.loadFrom(sumObj);

  }

}

// eof
