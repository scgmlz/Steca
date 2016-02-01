// common and useful definitions

#ifndef DEFS_H
#define DEFS_H

#include "core_defs.h"

// exceptions
class CriticalError: public Exception {
};

class QObject;
void warn(QObject*, rcstr msg, rcstr more = EMPTY_STR);

// TODO temporary wait solution

class WaitCursor {
public:
  WaitCursor();
 ~WaitCursor();
};

// TODO is it built in?

class BoolGuard {
public:
  BoolGuard(bool &);
 ~BoolGuard();
private:
  bool &b;
};

#endif
