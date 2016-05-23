// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      actions.h
//! @brief     A structure with all (glbal) actions
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef ACTIONS_H
#define ACTIONS_H

#include "refhub.h"
#include "types/core_defs.h"
#include <QAction>

namespace gui {
//------------------------------------------------------------------------------

class Action : public QAction {
  SUPER(Action, QAction)
public:
  Action(rcstr text, rcstr tip, QObject*);

  Action& text(rcstr, bool alsoTip = false);
  Action& tip(rcstr);
  Action& key(QKeySequence);
  Action& icon(rcstr);

  virtual Action& alt(rcstr text2, rcstr tip2);
};

class TriggerAction : public Action {
  SUPER(TriggerAction, Action)
public:
  TriggerAction(rcstr text, rcstr tip, QObject*);
};

class ToggleAction : public Action {
  SUPER(ToggleAction, Action)
public:
  ToggleAction(rcstr text, rcstr tip, QObject*);
  Action& alt(rcstr text2, rcstr tip2 = EMPTY_STR);

protected:
  str text1_, text2_, tip1_, tip2_;
};

//------------------------------------------------------------------------------

class Actions : protected RefHub {
  SUPER(Actions, RefHub)
public:
  Actions(TheHub&);

  Action
      *about, *quit,
      *viewStatusbar, *viewFiles, *viewDatasets, *viewDatasetInfo, *viewReset,
#ifndef Q_OS_OSX  // Mac has its own
      *fullScreen,
#endif
      *loadSession, *saveSession,
      *addFiles, *remFile, *enableCorr, *remCorr,
      *rotateImage, *mirrorImage, *linkCuts, *showCut, *hasBeamOffset,
      *fixedIntenImageScale, *fixedIntenDgramScale, *combinedDgram,
      *fitRegions, *fitBgClear, *fitBgShow,
      *addReflection, *remReflection,
      *outputPolefigures, *outputHistograms;

private:
  Action& trg(Action*& action, rcstr text, rcstr tip = EMPTY_STR);
  Action& tgl(Action*& action, rcstr text, rcstr tip = EMPTY_STR);
};

//------------------------------------------------------------------------------
}
#endif  // ACTIONS_H
