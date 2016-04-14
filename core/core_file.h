// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_file.h
//! @brief     File with datasets.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_FILE_H
#define CORE_FILE_H

#include "core_defs.h"
#include "core_dataset.h"
#include <QFileInfo>

namespace core {
//------------------------------------------------------------------------------

/// A file (loaded from a disk file) that contains a number of datasets.
class File final {
public:
  File(rcstr fileName);
 ~File();

  QFileInfo const& fileInfo() const { return info;              }
  str  fileName()             const { return info.fileName();   }
  uint numDatasets()          const { return datasets.count();  }

  void fold(); ///< collapse datasets into one (for correction files)

  shp_Dataset const& getDataset(uint i) const { return datasets.at(i); }
  void appendDataset(Dataset* dataset); ///< takes ownership of dataset

  /// all datasets contain images of the same size
  QSize getImageSize() const;
  /// the range of all intensities in all datasets
  Range const& intensRange() const;

private:
  QFileInfo info;
  QVector<shp_Dataset> datasets;
  mutable Range rgeIntens;

public:
  qreal calAverageMonitor()   const;
  qreal calAverageDeltaTime() const;
};

//------------------------------------------------------------------------------
}

Q_DECLARE_METATYPE(core::shp_File)

#endif // CORE_FILE_H
