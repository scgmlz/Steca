// common and useful definitions

#ifndef DEFS_H
#define DEFS_H

#include "core_defs.h"

// a class definition helper: this class and superclass access
#define SUPER(cls,sup)  typedef cls thisCls; typedef sup super;

// iteration
#define for_i(num)          for (int i=0, iEnd=(num); i<iEnd; ++i)
#define for_i_reverse(num)  for (int i=(num); i-->0; )

// exceptions

class CriticalError: public Exception {
};

class QObject;
void warn(QObject*, rcstr msg, rcstr more = NULL_STR);

// TODO temporary wait solution

class WaitCursor {
public:
  WaitCursor();
 ~WaitCursor();
};

#endif
