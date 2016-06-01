// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      thehub.h
//! @brief     The communication hub.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef THEHUB_H
#define THEHUB_H

#include "actions.h"
#include "core_session.h"
#include "core_session.h"
#include "models.h"
#include "types/core_defs.h"
#include <QAction>
#include <QSettings>

class QSpinBox;
class QDoubleSpinBox;
class Progress;

namespace gui {
//------------------------------------------------------------------------------

class Settings : public QSettings {
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

  qreal readReal(rcstr key, qreal def);
  void  saveReal(rcstr key, qreal);
};

//------------------------------------------------------------------------------

class ReadFile: public QFile {
  SUPER(ReadFile, QFile)
public:
  ReadFile(rcstr path) THROWS;
};

class WriteFile: public QFile {
  SUPER(WriteFile, QFile)
public:
  WriteFile(rcstr path) THROWS;
};

//------------------------------------------------------------------------------

class TheHub : public TheHubSignallingBase {
  SUPER(TheHub, TheHubSignallingBase)
  friend class TheHubSignallingBase;
public:
  TheHub();

  Actions actions;

private:
  void configActions();

private:
  QScopedPointer<core::Session> session;

  bool isFixedIntenImageScale_;
  bool isFixedIntenDgramScale_;
  bool isCombinedDgram_;

public:
  bool isFixedIntenImageScale() const { return isFixedIntenImageScale_; }
  bool isFixedIntenDgramScale() const { return isFixedIntenDgramScale_; }
  bool isCombinedDgram()        const { return isCombinedDgram_;        }

  models::FilesModel       filesModel;
  models::DatasetsModel    datasetsModel;
  models::ReflectionsModel reflectionsModel;

public:  // files
  uint numFiles() const;
  str fileName(uint index) const;
  str filePath(uint index) const;
  core::shp_File getFile(uint) const;
  void           remFile(uint);

  bool          hasCorrFile() const;
  core::rcImage corrImage() const;

public:
  core::shp_ImageLens lensNoCut(core::rcImage) const;

  core::shp_Lens lens(core::rcDataset) const;
  core::shp_Lens lensNoCut(core::rcDataset) const;

  core::AngleMap const& angleMap(core::rcDataset) const;

public:
  core::ReflectionInfos makeReflectionInfos(core::rcReflection,
      core::deg betaStep, core::rcRange rgeGamma, Progress* = nullptr);

public:
  void       saveSession(QFileInfo const&) const;
  QByteArray saveSession() const;

  void loadSession(QFileInfo const&) THROWS;
  void loadSession(QByteArray const&) THROWS;

public:
  void addFile(rcstr filePath) THROWS;
  void addFiles(str_lst const& filePaths) THROWS;

private:
  uint_vec collectFromFiles_;
  uint     datasetsGroupedBy_;

public:
  void collectDatasetsFromFiles(uint_vec, uint);
  void collectDatasetsFromFiles(uint_vec);
  void combineDatasetsBy(uint);

  uint_vec const& collectedFromFiles() const {
    return session->collectedFromFiles();
  }

  uint datasetsGroupedBy() const { return  datasetsGroupedBy_; }

  uint numCollectedDatasets() const {
    return collectedDatasets().count();
  }

  core::rcDatasets collectedDatasets() const {
    return session->collectedDatasets();
  }

  str_lst const& collectedDatasetsTags() const {
    return session->collectedDatasetsTags();
  }

  void setCorrFile(rcstr filePath) THROWS;
  void tryEnableCorrection(bool);

  core::ImageCut const& imageCut() const;
  void setImageCut(bool topLeft, bool linked, core::ImageCut const&);

  core::Geometry const& geometry() const;
  void setGeometry(qreal detectorDistance, qreal pixSize, bool isMidPixOffset,
                   core::rcIJ midPixOffset);

  void setBgRanges(core::rcRanges);
  void addBgRange(core::rcRange);
  void remBgRange(core::rcRange);

  static uint constexpr MAX_POLYNOM_DEGREE = 4;
  void setBgPolyDegree(uint);

  void setReflType(core::ePeakType);

  void addReflection(core::ePeakType);
  void remReflection(uint);

  eFittingTab fittingTab() const { return fittingTab_; }
  void setFittingTab(eFittingTab);

private:
  eFittingTab fittingTab_;

private:
  core::shp_Dataset    selectedDataset_;
  core::shp_Reflection selectedReflection_;

public:
  core::shp_Dataset selectedDataset() const { return selectedDataset_; }

private:
  void setImageRotate(core::ImageTransform);
  void setImageMirror(bool);

public:
  void setNorm(core::eNorm);

public:
  core::rcRanges bgRanges()         const { return session->bgRanges();     }
  uint           bgPolyDegree()     const { return session->bgPolyDegree(); }

  core::rcReflections reflections() const { return session->reflections();  }
};

//------------------------------------------------------------------------------
}
#endif
