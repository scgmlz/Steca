// ************************************************************************** //
//
//  Steca2:    StressTextureCalculator ver. 2
//
//! @file      thehub.h
//! @brief     The communication hub.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef THEHUB_H
#define THEHUB_H

#include "actions.h"
#include "session.h"
#include "models.h"
#include "def/defs.h"

#include <QAction>
#include <QSettings>

class QSpinBox;
class QDoubleSpinBox;
class Progress;

namespace gui {
//------------------------------------------------------------------------------

class Settings : public QSettings {
  CLS(Settings) SUPER(QSettings)
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
  CLS(ReadFile) SUPER(QFile)
public:
  ReadFile(rcstr path) THROWS;
};

class WriteFile: public QFile {
  CLS(WriteFile) SUPER(QFile)
public:
  WriteFile(rcstr path) THROWS;
};

//------------------------------------------------------------------------------

class TheHub : public TheHubSignallingBase {
  CLS(TheHub) SUPER(TheHubSignallingBase)
  friend class TheHubSignallingBase;
public:
  TheHub();

  Actions actions;

private:
  void configActions();

private:
  scoped<core::Session*> session;

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
  data::shp_File getFile(uint) const;
  void           remFile(uint);

  bool          hasCorrFile() const;
  typ::Image::rc corrImage() const;

public:
  calc::shp_ImageLens   plainImageLens(typ::Image::rc) const;
  calc::shp_DatasetLens datasetLens(data::Dataset::rc) const;

public:
  calc::ReflectionInfos makeReflectionInfos(calc::Reflection::rc,
      gma_t gmaStep, gma_rge::rc, Progress* = nullptr);

public:
  void       saveSession(QFileInfo const&) const;
  QByteArray saveSession() const;

  void loadSession(QFileInfo const&) THROWS;
  void loadSession(QByteArray const&) THROWS;

public:
  void addFile(rcstr filePath) THROWS;
  void addFiles(str_lst::rc filePaths) THROWS;

private:
  uint_vec collectFromFiles_;
  pint     datasetsGroupedBy_ = pint(1);

public:
  void collectDatasetsFromFiles(uint_vec, pint);
  void collectDatasetsFromFiles(uint_vec);
  void combineDatasetsBy(pint);

  uint_vec::rc collectedFromFiles() const {
    return session->collectedFromFiles();
  }

  pint datasetsGroupedBy() const {
    return datasetsGroupedBy_;
  }

  uint numCollectedDatasets() const {
    return collectedDatasets().count();
  }

  data::Datasets::rc collectedDatasets() const {
    return session->collectedDatasets();
  }

  str_lst::rc collectedDatasetsTags() const {
    return session->collectedDatasetsTags();
  }

  void setCorrFile(rcstr filePath) THROWS;
  void tryEnableCorrection(bool);

  typ::ImageCut::rc imageCut() const;
  void setImageCut(bool topLeft, bool linked, typ::ImageCut::rc);

  typ::Geometry::rc geometry() const;
  void setGeometry(preal detectorDistance, preal pixSize,
                   bool isMidPixOffset, typ::IJ::rc midPixOffset);

  void setBgRanges(typ::Ranges::rc);
  void addBgRange(typ::Range::rc);
  void remBgRange(typ::Range::rc);

  static uint constexpr MAX_POLYNOM_DEGREE = 4;
  void setBgPolyDegree(uint);

  void setReflType(fit::ePeakType);

  void addReflection(fit::ePeakType);
  void remReflection(uint);

  eFittingTab fittingTab() const { return fittingTab_; }
  void setFittingTab(eFittingTab);

private:
  eFittingTab fittingTab_;

private:
  data::shp_Dataset    selectedDataset_;
  calc::shp_Reflection selectedReflection_;

public:
  data::shp_Dataset selectedDataset() const { return selectedDataset_; }

private:
  void setImageRotate(typ::ImageTransform);
  void setImageMirror(bool);

public:
  void setNorm(eNorm);

public:
  typ::Ranges::rc bgRanges()          const { return session->bgRanges();     }
  uint            bgPolyDegree()      const { return session->bgPolyDegree(); }

  calc::Reflections::rc reflections() const { return session->reflections();  }
};

//------------------------------------------------------------------------------
}
#endif
