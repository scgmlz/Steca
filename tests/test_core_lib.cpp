#include "test_core_lib.h"

#include "core_types.h"
#include <core_file.h>
#include <core_session.h>

void TestCoreLib::testConversions() {
  QCOMPARE(M_PI,core::rad_deg(180));
  QCOMPARE(90.0,core::deg_rad(M_PI_2));

  for_i (720)
    QCOMPARE((qreal)i,core::deg_rad(core::rad_deg(i)));
}

void TestCoreLib::testFile() {
  {//using default Constructor
    core::File f(EMPTY_STR);
    QByteArray b;
    QCOMPARE(f.getImageSize(),QSize(0,0));
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
