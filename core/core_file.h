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

  QFileInfo const& fileInfo() const { return _fileInfo;              }
  str fileName()              const { return _fileInfo.fileName();   }

  Datasets&  datasets()             { return _datasets;              }
  rcDatasets datasets()       const { return _datasets;              }

private:
  QFileInfo _fileInfo;
  Datasets  _datasets;
};

//------------------------------------------------------------------------------
}

Q_DECLARE_METATYPE(core::shp_File)

#endif // CORE_FILE_H
