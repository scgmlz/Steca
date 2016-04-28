// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      thehub.h
//! @brief     The communication hub.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef THEHUB_H
#define THEHUB_H

#include "core_defs.h"
#include "actions.h"
#include "models.h"
#include "core_session.h"
#include <QAction>
#include <QSettings>

class QSpinBox;
class QDoubleSpinBox;

namespace gui {
//------------------------------------------------------------------------------

class WaitCursor {
public:
  WaitCursor();
 ~WaitCursor();
};

//------------------------------------------------------------------------------

class Settings: public QSettings {
  SUPER(Settings, QSettings)
public:
  Settings(rcstr group = EMPTY_STR);
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

  Actions actions;

private:
  void initActions();
  void configActions();

private:
  QScopedPointer<core::Session> session;

public:
  bool fixedIntenScaleImage_;
  bool fixedIntenScaleDgram_;
  bool avgCurveDgram_;

  models::FilesViewModel      fileViewModel;
  models::DatasetViewModel    datasetViewModel;
  models::ReflectionViewModel reflectionViewModel;

public: // files
  uint numFiles()              const;
  str  fileName(uint index)    const;
  str  filePath(uint index)    const;
  core::shp_File getFile(uint) const;
  void remFile(uint);

  bool hasCorrFile()           const;
  core::rcImage corrImage()    const;

  // emit signals
  void tellFilesSelectedDatasetsChanged();
  void tellSelectedDataset(core::shp_Dataset);
  void tellSelectedReflection(core::shp_Reflection);
  void tellReflectionData(core::shp_Reflection);
  void tellReflectionData(core::rcRange,core::rcXY,qreal,bool);

public:
  core::shp_ImageLens lensNoCut(core::rcImage)   const;

  core::shp_Lens      lens(core::rcDataset)      const;
  core::shp_Lens      lensNoCut(core::rcDataset) const;

  core::AngleMap const& angleMap(core::rcDataset)  const;

public:
  core::ReflectionInfos reflectionInfos(core::rcReflection, qreal betaStep,
                                        core::rcRange gammaRange = core::Range());

public:
  void saveSession(QFileInfo const&) const;
  QByteArray saveSession()           const;

  void loadSession(QFileInfo const&)  THROWS;
  void loadSession(QByteArray const&) THROWS;

public:
  void addFile(rcstr filePath)      THROWS;
  void addFiles(str_lst filePaths)  THROWS;

  void collectDatasetsFromFiles(uint_vec);
  uint_vec const& collectedFromFiles() const { return session->collectedFromFiles();   }
  core::rcDatasets collectedDatasets() const { return session->collectedDatasets();    }

  void setCorrFile(rcstr filePath)  THROWS;
  void enableCorrection(bool);

  core::ImageCut const& imageCut() const;
  void setImageCut(bool topLeft, bool linked, core::ImageCut const&);

  core::Geometry const& geometry() const;
  void setGeometry(qreal detectorDistance, qreal pixSize, bool isMidPixOffset, core::rcIJ midPixOffset);

  void setBackgroundPolynomialDegree(uint);

  void setReflType(core::ePeakType);

  void addReflection(core::ePeakType);
  void remReflection(uint);

  enum {
    TAB_BACKGROUND,
    TAB_REFLECTIONS,
  };

  int fittingTab__; // TODO
  void setFittingTab(int);

private:
  core::shp_Reflection selectedReflection;

private:
  void setImageRotate(core::ImageTransform);
  void setImageMirror(bool);

public:
  void setNorm(core::eNorm);

signals:
  void readSettings();
  void saveSettings();

  void filesChanged();
  void filesSelected();
  void corrEnabled(bool);
  void corrFileName(QString const&);

  void datasetsChanged();
  void datasetSelected(core::shp_Dataset);

  void reflectionsChanged();
  void reflectionSelected(core::shp_Reflection);
  void reflectionData(core::shp_Reflection);
  void reflectionValues(core::Range const&, core::XY const&, qreal, bool);

  void displayChange();
  void geometryChanged();

  void backgroundPolynomialDegree(uint);

  void normChanged();

  void fittingTab(int);

public:
  // TODO instead of exposing the objects, provide an interface
  core::Ranges&       bgRanges()           const { return session->bgRanges();           }
  uint&               bgPolynomialDegree() const { return session->bgPolynomialDegree(); }
  core::Reflections&  reflections()        const { return session->reflections();        }
};

#define ON_HUB_SIGNAL(signal,lambda) connect(&hub_, &TheHub::signal, [this]lambda);

//------------------------------------------------------------------------------
}
#endif
