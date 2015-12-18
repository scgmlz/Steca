#include "action.h"

Action::Action(rcstr text, QObject* parent, bool checkable,
               QKeySequence const& shortcut): super(text,parent) {
  setShortcut(shortcut);
  setCheckable(checkable);
}

Action* Action::trigger(rcstr text, QObject* receiver, char const* slot,
               QKeySequence const& shortcut) {
  auto act = new Action(text,receiver,false,shortcut);
  connect(act, SIGNAL(triggered()), receiver, slot);
  return act;
}

Action* Action::toggle(rcstr text, QObject* receiver, char const* slot,
               QKeySequence const& shortcut) {
  auto act = new Action(text,receiver,true,shortcut);
  connect(act, SIGNAL(toggled(bool)), receiver, slot);
  return act;
}

// eof
