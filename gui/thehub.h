/** \file
 */

#ifndef THEHUB_H
#define THEHUB_H

#include "core_defs.h"
#include <QAction>
#include "models.h"

//------------------------------------------------------------------------------

namespace core {
class Session;
}

//------------------------------------------------------------------------------

class Action: public QAction {
  SUPER(Action,QAction)
public:
  Action(rcstr text, rcstr tip, rcstr iconFile, QObject*);
};

class PushAction: public Action {
  SUPER(PushAction,Action)
public:
  PushAction(rcstr text, rcstr tip, rcstr iconFile, QObject*);
};

class ToggleAction: public Action {
  SUPER(ToggleAction,Action)
public:
  ToggleAction(rcstr text, rcstr tip, rcstr iconFile, QObject*);
};

//------------------------------------------------------------------------------

class TheHub: public QObject {
  SUPER(TheHub,QObject)
public:
  TheHub();
 ~TheHub();

private:
  void initActions();

private:
  core::Session *session;
  bool globalNorm;  // TODO rename this and related to fixedIntensityScale
  model::FileViewModel       fileViewModel;    // TODO not public
  model::DatasetViewModel    datasetViewModel;
  model::ReflectionViewModel reflectionViewModel;

public:
  QAction
    *actAddFiles, *actRemoveFile,
    *actLoadCorrectionFile,
    *actLoadSession, *actSaveSession,

    *actExportDiffractogramCurrent,
    *actExportDiffractogramAllSeparateFiles,
    *actExportDiffractogramSingleFile,
    *actExportImagesWithMargins,
    *actExportImagesWithoutMargins,

    *actQuit,

    *actUndo, *actRedo,
    *actCut,  *actCopy, *actPaste,

    *actViewStatusbar,
#ifndef Q_OS_OSX // Mac has its own
    *actFullscreen,
#endif
    *actViewReset,

    *actReflectionSelectRegion,
    *actCalculatePolefigures,
    *actCalculateHistograms,
    *actPreferences,
    *actFitErrorParameters,

    *actAbout,

  // more actions, some not in the menu
    *actSelectPeak, *actReflectionPeak, *actReflectionWidth, *actReflectionAdd, *actReflectionRemove,
    *actImageRotate, *actImageMirror,
    *actImagesLink, *actImageOverlay, *actImagesGlobalNorm, *actImagesEnableCorr,
    *actBackgroundClear, *actBackgroundBackground, *actBackgroundShowFit,
    *actHasBeamOffset,
    *actNormalizationDisable, *actNormalizationMeasureTime, *actNormalizationMonitor, *actNormalizationBackground;

public: // files
  uint numFiles(bool withCorr) const;
  bool hasCorrFile()           const;
  bool isCorrFile(uint index)  const;
  str  fileName(uint index)    const;
  str  filePath(uint index)    const;

  core::shp_File getFile(uint i)  const;
  void remFile(uint i);

  void setSelectedFile(core::shp_File);
  void setSelectedDataset(core::shp_Dataset);
};

//------------------------------------------------------------------------------
#endif
