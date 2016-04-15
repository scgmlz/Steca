// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      actions.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "actions.h"

namespace gui {
//------------------------------------------------------------------------------

Action::Action(rcstr text, rcstr tip, QObject* parent): super(text,parent) {
  setToolTip(tip.isEmpty() ? text : tip);
}

Action&Action::dialog() {
  setText(text() + "…");
  return *this;
}

Action& Action::key(QKeySequence key) {
  setShortcut(key);
  return *this;
}

Action& Action::icon(rcstr iconFile) {
  setIcon(QIcon(iconFile));
  return *this;
}

Action& Action::alt(rcstr /*text2*/, rcstr /*tip2*/) {
  return *this;
}

//------------------------------------------------------------------------------

TriggerAction::TriggerAction(rcstr text, rcstr tip, QObject* parent)
: super(text,tip,parent) {
}

//------------------------------------------------------------------------------

ToggleAction::ToggleAction(rcstr text, rcstr tip, QObject* parent)
  : super(text,tip,parent), text1_(text), tip1_(tip.isEmpty() ? text : tip) {
  setCheckable(true);
}

Action& ToggleAction::alt(rcstr text2, rcstr tip2) {
  text2_ = text2; tip2_ = tip2.isEmpty() ? text2 : tip2;
  connect(this,&thisClass::toggled,[this](bool on) {
    setText(on ? text2_ : text1_);
    setToolTip(on ? tip2_ : tip1_);
  });

  return super::alt(text2, tip2);
}

//------------------------------------------------------------------------------
}
// eof
