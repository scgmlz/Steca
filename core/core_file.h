#ifndef CORE_FILE_H
#define CORE_FILE_H

#include "core_defs.h"
#include "core_dataset.h"
#include <QVector>
#include <QFileInfo>
#include <QMetaType>

namespace core {

class File final {
public:
  File();
  File(rcstr fileName);
 ~File();

  str name()                 const { return info.fileName(); }
  QFileInfo const& getInfo() const { return info;            }

  void load() THROWS;

  Datasets const& getDatasets() const { return datasets; }

private:
  QFileInfo info;
  Datasets  datasets;
};

class Files final: public QVector<File*> {
public:
  Files();
 ~Files();
};

}

// used in signals
typedef core::File const *pcCoreFile;
Q_DECLARE_METATYPE(pcCoreFile)

#endif
