/** \file
 * Files with datasets.
 */

#ifndef CORE_FILE_H
#define CORE_FILE_H

#include "core_defs.h"
#include "core_dataset.h"
#include <QFileInfo>
#include <QMetaType>

namespace core {
//------------------------------------------------------------------------------

/// A file (loaded from a disk file) that contains a number of datasets.
class File final {
public:
  File(rcstr fileName);
 ~File();

  QFileInfo const& getInfo() const { return info;            }
  str getName()              const { return info.fileName(); }

  void fold();        ///< collapse datasets into one

  uint numDatasets() const { return datasets.count(); }

  shp_Dataset const& getDataset(uint i) const { return datasets.at(i); }
  void appendDataset(Dataset* dataset); ///< takes ownership of dataset

  /// all datasets contain images of the same size
  QSize getImageSize() const;
  /// the range of all intensities in all datasets
  Range const& getRgeIntens() const;

private:
  QFileInfo info;
  QVector<shp_Dataset> datasets;
  mutable Range rgeIntens;

public:
  qreal const calAverageMonitor();
  qreal const calAverageDeltaTime();
};

//------------------------------------------------------------------------------
}

Q_DECLARE_METATYPE(core::shp_File)

#endif
