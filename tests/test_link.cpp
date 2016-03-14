#include "test_link.h"
#include "core_priority_chain.h"

TestLink::TestLink() {

}

void TestLink::nextChangedImpl() {

}

uint TestLink::getPriority() const {
  return PRIO;
}

void TestPriorityChain::testChainLink() {

  TestLink testLink;

}

