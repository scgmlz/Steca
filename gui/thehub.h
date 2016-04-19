// TODO HEADER
/** \file
 */

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
  bool fixedIntensityScale;

  models::FilesViewModel       fileViewModel;
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

  void setSelectedFile(core::shp_File);
  void setSelectedDataset(core::shp_Dataset);
  void setSelectedReflection(core::shp_Reflection);
  void setReflectionData(core::shp_Reflection);
  void newReflectionData(core::rcRange,core::rcXY,qreal,bool);

public:
  core::shp_ImageLens lensNoCut(core::rcImage)   const;

  core::shp_Lens      lens(core::rcDataset)      const;
  core::shp_Lens      lensNoCut(core::rcDataset) const;

  core::AngleMap const& angleMap(core::rcDataset)  const;

public:
  void saveSession(QFileInfo const&) const;
  QByteArray saveSession()           const;

  void loadSession(QFileInfo const&)  THROWS;
  void loadSession(QByteArray const&) THROWS;

public:
  void addFile(rcstr filePath)      THROWS;
  void addFiles(str_lst filePaths)  THROWS;

  core::Datasets& workingDatasets()     { return session->workingDatasets(); }

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
  void setNorm(core::Lens::eNorm);

signals:
  void readSettings();
  void saveSettings();

  void filesChanged();
  void corrEnabled(bool);
  void corrFileName(QString const&);

  void fileSelected(core::shp_File);
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

//------------------------------------------------------------------------------
}
#endif
