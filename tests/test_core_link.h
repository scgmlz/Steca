#ifndef TEST_CORE_LINK_H
#define TEST_CORE_LINK_H

#include "core_priority_chain.h"
#include <QtTest/QtTest>

//------------------------------------------------------------------------------

using namespace core;

enum TestLinkPriority {
  PRIO1, PRIO2, PRIO3, PRIO4, PRIO5, PRIO6
};

//------------------------------------------------------------------------------

class TestLink : public ChainLink<TestLink> {
public:
  TestLink();
  using shp_TestLinkSystem = QSharedPointer<TestLink>;

public:
  const static uint PRIO = TestLinkPriority::PRIO6; // has higher priority than DummyLink

  uint getPriority() const override;
  shp_TestLinkSystem getPrevious();

protected:
  void nextChangedImpl() override;
};

class DummyLink1 : public TestLink {
public:
  DummyLink1();

public:
  const static uint PRIO = TestLinkPriority::PRIO1; // hast lower priority than TestLink
  uint getPriority() const override;


protected:
  void nextChangedImpl() override;
};

class DummyLink2 : public TestLink {
public:
  DummyLink2();

public:
  const static uint PRIO = TestLinkPriority::PRIO2; // hast lower priority than TestLink
  uint getPriority() const override;


protected:
  void nextChangedImpl() override;
};

class DummyLink3 : public TestLink {
public:
  DummyLink3();

public:
  const static uint PRIO = TestLinkPriority::PRIO3; // hast lower priority than TestLink
  uint getPriority() const override;


protected:
  void nextChangedImpl() override;
};

//------------------------------------------------------------------------------

class TestCorePriorityChain: public QObject {
  Q_OBJECT

private slots:
  void testRecursiv1();
  void testRecursiv2();
  void testReplaceFirst();
  void testReOrder();
  void testInsertAsFirst();
  void testChainLink();

};

//------------------------------------------------------------------------------

#endif // TEST_CORE_LINK_H
