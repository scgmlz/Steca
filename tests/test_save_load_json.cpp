#include "test_save_load_json.h"
REGISTER_TEST_SUITE(TestSaveLoadJson)

#include "core_fit_functions.h"
#include "types/core_json.h"

// make non-public methods public
#define TEST_FIT_CLASS(cls)                 \
  class Test##cls: public core::fit::cls {  \
public:                                     \
  void setValue(uint parIndex, qreal val) { \
    core::fit::cls::setValue(parIndex,val); \
  }                                         \
};

TEST_FIT_CLASS(Polynom)
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
    core::fit::Function::Parameter p1;
    p1.setValueRange(r1,r1); // finite,finite
    p1.setValue(val1,error,true);
    core::JsonObj pObj = p1.saveJson();
    core::fit::Function::Parameter p3;
    p3.loadJson(pObj);
    QCOMPARE(p3.valueRange().min, r1);
    QCOMPARE(p3.valueRange().max, r1);
    QCOMPARE(p3.value(), val1);

    core::fit::Function::Parameter p2;
    p2.setValue(val1,error,true);
    pObj = p2.saveJson();
    core::fit::Function::Parameter p4;
    p4.loadJson(pObj);
    QVERIFY(qIsInf(p4.valueRange().min));
    QVERIFY(qIsInf(p4.valueRange().max));
    QCOMPARE(p4.value(), val1);
  }

  { // testing saveJson/loadJson Polynom
    TestPolynom polyLoad;
    TestPolynom polySave;

    polySave.setDegree(3);
    polySave.setParameterCount(4);
    polySave.setValue(0,val1);
    polySave.setValue(1,val2);
    polySave.setValue(2,val3);
    polySave.setValue(3,val4);

    core::JsonObj polyObj = polySave.saveJson();

    QCOMPARE(polyLoad.degree(),0u);
    QCOMPARE(polyLoad.parameterCount(),1u);
    QCOMPARE(polyLoad.parameterAt(0).value(),val0);

    polyLoad.loadJson(polyObj);

    QCOMPARE(polyLoad.degree(),3u);
    QCOMPARE(polyLoad.parameterCount(),4u);
    QCOMPARE(polyLoad.parameterAt(0).value(),val1);


    // testing saveJson/loadJson CauchyLorentz
    TestCauchyLorentz cauchySave, cauchyLoad;
    cauchySave.setValue(0,val1);
    cauchySave.setValue(1,val2);

    core::JsonObj cObj = cauchySave.saveJson();
    cauchyLoad.loadJson(cObj);

    QCOMPARE(cauchyLoad.parameterCount(),3u);
    QCOMPARE(cauchyLoad.parameterAt(0).value(),val1);

    TestPseudoVoigt1 pseudoSave, pseudoLoad;
    pseudoSave.setValue(0,val4);

    core::JsonObj psObj = pseudoSave.saveJson();
    pseudoLoad.loadJson(psObj);

    QCOMPARE(pseudoLoad.parameterCount(),4u);
    QCOMPARE(pseudoLoad.parameterAt(0).value(),val4);
    bool verify = false;
    auto max = pseudoLoad.parameterAt(0).valueRange().max;
    if ( qIsInf(max))
      verify = (max > 0) ? true : false;

    QVERIFY(verify);

    // testing saveJson/loadJson Gaussian

    TestGaussian gSave, gLoad;
    core::JsonObj gObj;
    gSave.setValue(0,val3);

    // Testing saveJson/loadJson SumFunctions

    core::fit::SumFunctions sumSave, sumLoad;
    TestPolynom *p1 = new TestPolynom, *p2 = new TestPolynom;

    p1->loadJson(polyObj);
    p2->loadJson(polyObj);

    sumSave.addFunction(p1);
    sumSave.addFunction(p2);

    core::JsonObj sumObj = sumSave.saveJson();
    sumLoad.loadJson(sumObj);
  }
}

// eof
