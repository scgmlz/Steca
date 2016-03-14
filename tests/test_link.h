#ifndef TEST_LINK_H
#define TEST_LINK_H

#include "core_priority_chain.h"
#include "core_lens.h"

#include <QtTest/QtTest>

#define TEST_UNIT_TESTS

//------------------------------------------------------------------------------

using namespace core;

//------------------------------------------------------------------------------

enum TestLinkPriority {
  PRIO1, PRIO2, PRIO3, PRIO4, PRIO5, PRIO6
};

//------------------------------------------------------------------------------

class TestLink final : public ChainLink<TestLink> {
public:
  TestLink();

public:
  const static uint PRIO = TestLinkPriority::PRIO6;

  uint getPriority() const override;


protected:
  void nextChangedImpl() override;
};

//------------------------------------------------------------------------------

class TestPriorityChain: public QObject {
  Q_OBJECT

private slots:
  void testChainLink();

};

//------------------------------------------------------------------------------

#endif
