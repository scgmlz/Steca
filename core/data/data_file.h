// ************************************************************************** //
//
// STeCa2 - StressTextureCalculator ver. 2
//
// Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the LICENSE and AUTHORS files for more details.
// ************************************************************************** //

#ifndef FWD_DATA_FILE_H
#define FWD_DATA_FILE_H

#include <QSharedPointer>

namespace data {

class File;
typedef QSharedPointer<File> shp_File;

}

Q_DECLARE_METATYPE(data::shp_File)

#endif

#ifndef FORWARD_DECLARATIONS
#ifndef DATA_FILE_H
#define DATA_FILE_H

#include "data_dataset.h"
#include <QFileInfo>

namespace data {

class Datasets;
//------------------------------------------------------------------------------

// A file (loaded from a disk file) that contains a number of datasets.
class File final {
  CLS(File)
public:
  File(rcstr fileName);

  QFileInfo const& fileInfo() const;
  str              fileName() const;

  OneDatasets&     datasets()       { return datasets_; }
  OneDatasets::rc  datasets() const { return datasets_; }

private:
  QFileInfo   fileInfo_;
  OneDatasets datasets_;
};

//------------------------------------------------------------------------------
}
#endif // DATA_FILE_H
#endif // FORWARD_DECLARATIONS
