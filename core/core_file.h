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

/// A single file with a number of datasets.
class File final {
public:
  File();
  File(rcstr fileName);
 ~File();

  str getName()              const { return info.fileName(); }
  QFileInfo const& getInfo() const { return info;            }

  /// peek at up to maxLen bytes (to establish the file type)
  QByteArray peek(uint maxLen);

  void load() THROWS; ///< load disk file content
  void fold();        ///< collapse datasets into one

  uint numDatasets() const { return datasets.count(); }

  shp_Dataset const& getDataset(uint i) const { return datasets.at(i); }

  uint getImageSize() const;

private:
  QFileInfo info;
  Dataset_vec datasets;
};

typedef QSharedPointer<File> shp_File;

}

Q_DECLARE_METATYPE(core::shp_File)

#endif
