#ifndef ACTION_H
#define ACTION_H

#include "defs.h"
#include <QAction>

class Action: public QAction {
  INHERIT_SUPER(Action,,QAction)
public:
  Action(rcstr text, QObject* parent, bool checkable,
         QKeySequence const& = 0);

  static Action* trigger(rcstr text,
         QObject* receiver, char const* slot,
         QKeySequence const& = 0);

  static Action* toggle(rcstr text,
         QObject* receiver, char const* slot,
         QKeySequence const& = 0);
};

#endif
