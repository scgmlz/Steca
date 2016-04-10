#include "test_core_lib.h"

#include "core_defs.h"
#include "core_file.h"
#include "core_session.h"

void TestCoreLib::testConversions() {
  QCOMPARE(M_PI,deg2Rad(180));
  QCOMPARE(90.0,rad2Deg(M_PI_2));

  for_i (720)
    QCOMPARE((qreal)i,rad2Deg(deg2Rad(i)));
}

void TestCoreLib::testFile() {
  {//using default Constructor
    core::File f(EMPTY_STR);
    QByteArray b;
    QCOMPARE(f.datasets().imageSize(),QSize(0,0));
  }
}

void TestCoreLib::testSession() {
  //Session
  {
    core::Session s;
    s.addFile(""); // expect no exception because filename not given
    rcstr name("TestFile");
    core::File f(name);

    //Filetype is not know, expected to throw
    bool check = false;
    try{
      s.addFile(name);
    }
    catch(Exception e) {
      check=true;
    }
    QVERIFY(check);
  }
}

// eof
