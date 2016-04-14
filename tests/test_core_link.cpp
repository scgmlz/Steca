#include "test_core_link.h"

using namespace core;
using shp_TestLinkSystem = QSharedPointer<TestLink>;

TestLink::TestLink() {
}

void TestLink::nextChangedImpl() {
}

uint TestLink::getPriority() const {
  return PRIO;
}

shp_TestLinkSystem TestLink::getPrevious() {
  return previous;
}

DummyLink1::DummyLink1() {
}

void DummyLink1::nextChangedImpl() {
}

uint DummyLink1::getPriority() const {
  return PRIO;
}

DummyLink2::DummyLink2() {
}

void DummyLink2::nextChangedImpl() {
}

uint DummyLink2::getPriority() const {
  return PRIO;
}

DummyLink3::DummyLink3() {
}

void DummyLink3::nextChangedImpl() {
}

uint DummyLink3::getPriority() const {
  return PRIO;
}

void TestCorePriorityChain::testRecursiv1() {
  auto dummyChain = shp_TestLinkSystem(new DummyLink1());
  dummyChain << shp_TestLinkSystem(new DummyLink2());
  auto testChain = shp_TestLinkSystem(new TestLink());
  testChain << dummyChain;

  QCOMPARE(testChain->getPriority(), (uint) TestLinkPriority::PRIO1);
  auto next = testChain->getNext();
  QCOMPARE(next->getPriority(), (uint) TestLinkPriority::PRIO2);
  QCOMPARE(next->getNext()->getPriority(), (uint) TestLinkPriority::PRIO6);

}

void TestCorePriorityChain::testRecursiv2() {
  auto dummyChain = shp_TestLinkSystem(new DummyLink3());
  dummyChain << shp_TestLinkSystem(new DummyLink1());

  QCOMPARE(dummyChain->getPriority(), (uint) TestLinkPriority::PRIO1);
  QCOMPARE(dummyChain->getNext()->getPriority(), (uint) TestLinkPriority::PRIO3);

  auto testChain = shp_TestLinkSystem(new TestLink());
  testChain << shp_TestLinkSystem(new DummyLink2());

  QCOMPARE(testChain->getPriority(), (uint) TestLinkPriority::PRIO2);
  QCOMPARE(testChain->getNext()->getPriority(), (uint) TestLinkPriority::PRIO6);

  testChain << dummyChain;

  QCOMPARE(testChain->getPriority(), (uint) TestLinkPriority::PRIO1);
  auto next = testChain->getNext();
  QCOMPARE(next->getPriority(), (uint) TestLinkPriority::PRIO2);
  auto third = next->getNext();
  QCOMPARE(third->getPriority(), (uint) TestLinkPriority::PRIO3);
  auto last = third->getNext();
  QCOMPARE(last->getPriority(), (uint) TestLinkPriority::PRIO6);

}

void TestCorePriorityChain::testReplaceFirst() {
  { // priority of link == firstLink
    auto testChain = shp_TestLinkSystem(new TestLink());
    testChain << shp_TestLinkSystem(new TestLink());

    QVERIFY(!testChain->hasNext());
    QCOMPARE(testChain->getPriority(), (uint) TestLinkPriority::PRIO6);
  }
}

void TestCorePriorityChain::testReOrder() {
  // priority of link > firstLink

  { // replace link
    auto testChain = shp_TestLinkSystem(new DummyLink1());
    testChain << shp_TestLinkSystem(new DummyLink2());
    testChain << shp_TestLinkSystem(new DummyLink3());
    testChain << shp_TestLinkSystem(new DummyLink2());

    auto first = testChain;
    auto second = first->getNext();
    auto third = second->getNext();
    QCOMPARE(first->getPriority(),   (uint) TestLinkPriority::PRIO1);
    QCOMPARE(second->getPriority(),  (uint) TestLinkPriority::PRIO2);
    QCOMPARE(third->getPriority(),   (uint) TestLinkPriority::PRIO3);
  }

  { // insert inbetween
    auto testChain = shp_TestLinkSystem(new DummyLink1()); // firstLink prio1
    testChain << shp_TestLinkSystem(new DummyLink3());
    testChain << shp_TestLinkSystem(new DummyLink2());

    auto first = testChain;
    auto second = first->getNext();
    auto third = second->getNext();
    QCOMPARE(first->getPriority(),   (uint) TestLinkPriority::PRIO1);
    QCOMPARE(second->getPriority(),  (uint) TestLinkPriority::PRIO2);
    QCOMPARE(third->getPriority(),   (uint) TestLinkPriority::PRIO3);
  }

  { // push in last
    auto testChain = shp_TestLinkSystem(new DummyLink1()); // firstLink prio1
    testChain << shp_TestLinkSystem(new TestLink()); // link prio6

    auto first = testChain;
    QCOMPARE(first->getPriority(), (uint) TestLinkPriority::PRIO1);
    QVERIFY(first->hasNext());
    auto prev = first->getPrevious();
    QVERIFY(prev == nullptr);

    auto next = first->getNext();
    QCOMPARE(next->getPriority(), (uint) TestLinkPriority::PRIO6);
    auto prevOfNext = next->getPrevious();
    QCOMPARE(prevOfNext->getPriority(),first->getPriority());
  }

}

void TestCorePriorityChain::testInsertAsFirst() {
  // priority of link < firstLink
  auto testChain = shp_TestLinkSystem(new TestLink());
  testChain << shp_TestLinkSystem(new DummyLink1());

  QCOMPARE(testChain->getPriority(), (uint) TestLinkPriority::PRIO1);
  QVERIFY(testChain->hasNext());
  auto next = testChain->getNext();
  QCOMPARE(next->getPriority(), (uint) TestLinkPriority::PRIO6);
  auto prev = testChain->getPrevious();
  QVERIFY(prev == nullptr);
}

void TestCorePriorityChain::testChainLink() {
  {
    auto testChain = shp_TestLinkSystem(nullptr);
    testChain << shp_TestLinkSystem(new TestLink());
    QVERIFY(testChain);
    QCOMPARE(testChain->getPriority(), (uint) TestLinkPriority::PRIO6);
  }

  {
    auto testChain = shp_TestLinkSystem(new TestLink());
    testChain->setNextSilent(shp_TestLinkSystem(new DummyLink1()));
    QVERIFY(testChain->hasNext());

    testChain->setNext(shp_TestLinkSystem(new DummyLink1()));
    QVERIFY(testChain->hasNext());
  }

  {
    auto testChain = shp_TestLinkSystem(new DummyLink1());
    testChain->setPrevious(shp_TestLinkSystem(new DummyLink2()));
    auto first = testChain;
    auto prev = first->getPrevious();
    QCOMPARE(first->getPriority(),   (uint) TestLinkPriority::PRIO1);
    QCOMPARE(prev->getPriority(),    (uint) TestLinkPriority::PRIO2);
  }
}
