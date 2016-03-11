#include "test_save_load_json.h"
#include "core_fit_functions.h"
#include "core_types.h"

void TestSaveLoadJson::testSaveLoadJson() {
  int error = 5;
  qreal r1 = 42;
  qreal val0 = 0;
  qreal val1 = 1;
  qreal val2 = 2;
  qreal val3 = 3;
  qreal val4 = 4;

  {
    QJsonObject pObj;
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

  using Polynomial = core::fit::Polynomial;
  { // testing saveTo/loadFrom Polynomial
    Polynomial polyLoad;
    Polynomial polySave;

    polySave.setDegree(3);
    polySave.setParameterCount(4);
    polySave.setValue(0,val1);
    polySave.setValue(1,val2);
    polySave.setValue(2,val3);
    polySave.setValue(3,val4);

    QJsonObject polyObj;

    polySave.saveTo(polyObj);

    QCOMPARE(polyLoad.getDegree(),0u);
    QCOMPARE(polyLoad.parameterCount(),1u);
    QCOMPARE(polyLoad.getParameter(0).getValue(),val0);

    polyLoad.loadFrom(polyObj);

    QCOMPARE(polyLoad.getDegree(),3u);
    QCOMPARE(polyLoad.parameterCount(),4u);
    QCOMPARE(polyLoad.getParameter(0).getValue(),val1);


    // testing saveTo/loadFrom CauchyLorentz
    core::fit::CauchyLorentz cauchySave, cauchyLoad;
    QJsonObject cObj;
    cauchySave.setValue(0,val1);
    cauchySave.setValue(1,val2);

    cauchySave.saveTo(cObj);
    cauchyLoad.loadFrom(cObj);

    QCOMPARE(cauchyLoad.parameterCount(),3u);
    QCOMPARE(cauchyLoad.getParameter(0).getValue(),val1);

    core::fit::PseudoVoigt1 pseudoSave, pseudoLoad;
    QJsonObject psObj;
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

    core::fit::Gaussian gSave, gLoad;
    QJsonObject gObj;
    gSave.setValue(0,val3);

    // Testing saveTo/loadFrom SumFunctions

    QJsonObject sumObj;

    core::fit::SumFunctions sumSave, sumLoad;
    Polynomial *p1 = new Polynomial, *p2 = new Polynomial;

    p1->loadFrom(polyObj);
    p2->loadFrom(polyObj);

    sumSave.addFunction(p1);
    sumSave.addFunction(p2);

    sumSave.saveTo(sumObj);
    sumLoad.loadFrom(sumObj);

  }

}

// eof
