/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef ACTIONS_H
#define ACTIONS_H

#include "refhub.h"
#include "def/defs.h"

#include <QAction>

namespace gui {
//------------------------------------------------------------------------------

class Action : public QAction {
  CLS(Action) SUPER(QAction)
public:
  Action(rcstr text, QObject*);

  Action& text(rcstr);
  Action& tip(rcstr);
  Action& key(QKeySequence);
  Action& icon(rcstr);

  virtual Action& alt(rcstr text2);
};

class TriggerAction : public Action {
  CLS(TriggerAction) SUPER(Action)
public:
  TriggerAction(rcstr text, QObject* = nullptr);
};

class ToggleAction : public Action {
  CLS(ToggleAction) SUPER(Action)
public:
  ToggleAction(rcstr text, QObject* = nullptr);

  Action& alt(rcstr text2);

protected:
  str text1_, text2_;
};

//------------------------------------------------------------------------------

class Actions : protected RefHub {
  CLS(Actions) SUPER(RefHub)
public:
  Actions(TheHub&);

  Action
    *about, *online, *checkUpdate, *quit,
    *viewStatusbar, *viewFiles, *viewDatasets, *viewDatasetInfo, *viewHelp, *viewReset,
#ifndef Q_OS_OSX  // Mac has its own
    *fullScreen,
#endif
    *loadSession, *saveSession, *clearSession,
    *addFiles, *remFile, *enableCorr, *remCorr,
    *rotateImage, *mirrorImage, *linkCuts, *showOverlay, *stepScale, *showBins,
    *fixedIntenImage, *fixedIntenDgram, *combinedDgram,
    *showAveraged,
    *selRegions, *showBackground, *clearBackground, *clearReflections,
    *addReflection, *remReflection,
    *outputPolefigures, *outputDiagrams, *outputDiffractograms;

private:
  Action& trg(Action*& action, rcstr text);
  Action& tgl(Action*& action, rcstr text);
};

//------------------------------------------------------------------------------
}
#endif // ACTIONS_H
