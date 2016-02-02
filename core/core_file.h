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

  QByteArray peek(uint maxLen); ///< peek at first up to maxLen bytes

  void load() THROWS;
  void sumDatasets();

  Datasets const& getDatasets() const { return datasets; }

private:
  QFileInfo info;
  Datasets  datasets;
};

}

/// a pointer that is used in signals
typedef core::File const *pcCoreFile;
Q_DECLARE_METATYPE(pcCoreFile)

#endif
