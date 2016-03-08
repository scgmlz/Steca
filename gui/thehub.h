/** \file
 */

#ifndef THEHUB_H
#define THEHUB_H

#include "core_defs.h"
#include "models.h"
#include "core_session.h"
#include <QAction>
#include <QSettings>
class QSpinBox;
class QDoubleSpinBox;

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
  ToggleAction(rcstr text1, rcstr text2, rcstr tip1, rcstr tip2, rcstr iconFile, QObject*);
  ToggleAction(rcstr text1, rcstr tip1, rcstr iconFile, QObject*);

protected:
  str text1, text2, tip1, tip2;
};

//------------------------------------------------------------------------------

class Settings: public QSettings {
  SUPER(Settings, QSettings)
public:
  Settings(rcstr group = "");
 ~Settings();

  QVariant readVariant(rcstr key, QVariant const& def);
  void     saveVariant(rcstr key, QVariant const& val);

  void read(rcstr key, QAction*, bool def);
  void save(rcstr key, QAction*);

  void read(rcstr key, QSpinBox*, int def);
  void save(rcstr key, QSpinBox*);

  void read(rcstr key, QDoubleSpinBox*, qreal def);
  void save(rcstr key, QDoubleSpinBox*);
};

//------------------------------------------------------------------------------

class TheHub: public QObject {
  SUPER(TheHub,QObject)
  Q_OBJECT
public:
  TheHub();
 ~TheHub();

private:
  void initActions();
  void configActions();

private:
  core::Session *session;

public:
  bool fixedIntensityScale;

  model::FileViewModel       fileViewModel;
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

    *actCalculatePolefigures,
    *actCalculateHistograms,
    *actPreferences,
    *actFitErrorParameters,

    *actAbout,

  // more actions, some not in the menu
    *actReflectionRegion, *actReflectionPeak, *actReflectionWidth, *actReflectionAdd, *actReflectionRemove,
    *actImageRotate, *actImageMirror,
    *actImagesLink, *actImageOverlay, *actImagesFixedIntensity, *actImagesEnableCorr,
    *actBackgroundClear, *actBackgroundBackground, *actBackgroundShowFit,
    *actHasBeamOffset,
    *actNormalizationDisable, *actNormalizationMeasureTime, *actNormalizationMonitor, *actNormalizationBackground;

public: // files
  uint numFiles(bool withCorr) const;
  bool hasCorrFile()           const;
  bool isCorrFile(uint index)  const;
  str  fileName(uint index)    const;
  str  filePath(uint index)    const;

  core::shp_File getFile(uint) const;
  void remFile(uint);

  void setSelectedFile(core::shp_File);
  void setSelectedDataset(core::shp_Dataset);
  void setSelectedReflection(core::shp_Reflection);

public:
  void load(QFileInfo const&)       THROWS;
  void load(QByteArray const& json) THROWS;
  QByteArray save() const;

public:
  void addFile(rcstr filePath)      THROWS;
  void addFiles(str_lst filePaths)  THROWS;
  void loadCorrFile(rcstr filePath);

  void enableCorrection(bool);

  core::ImageCut const& getImageCut() const;
  void setImageCut(bool topLeft, bool linked, core::ImageCut const&);

  QSize getImageSize() const;
  uint  pixIndexNoTransform(uint x, uint y) const;
  core::intens_t pixIntensity(core::Image const&, uint x, uint y) const; // REVIEW

  core::AngleCorrArray const& calcAngleCorrArray(qreal tthMitte);
  core::Borders const& getCut() const; // TODO somehow hide

  core::Geometry const& getGeometry() const;
  void setGeometry(qreal detectorDistance, qreal pixSize, bool hasBeamOffset, QPoint const& middlePixOffset);

  void setBackgroundPolynomialDegree(uint);

  core::Reflection::eType getReflType() const { return reflType; }
  void setReflType(core::Reflection::eType);

  void addReflection();
  void remReflection(uint);

private:
  core::shp_Reflection    selectedReflection;
  core::Reflection::eType reflType;

private:
  void setImageRotate(core::ImageTransform);
  void setImageMirror(bool);

signals:
  void readSettings();
  void saveSettings();

  void filesChanged();
  void correctionEnabled(bool);

  void fileSelected(core::shp_File);
  void datasetSelected(core::shp_Dataset);

  void reflectionsChanged();
  void reflectionSelected(core::shp_Reflection);

  void displayChange();
  void geometryChanged();

  void backgroundPolynomialDegree(uint);

public:
  // TODO instead of exposing the objects, provide an interface
  core::Ranges&           getBgRanges()     const { return session->getBgRanges();     }
  core::fit::Polynomial&  getBgPolynomial() const { return session->getBgPolynomial(); }
  core::Reflections&      getReflections()  { return session->getReflections();  }
};

//------------------------------------------------------------------------------
#endif
