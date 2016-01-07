#ifndef CORE_FILE_H
#define CORE_FILE_H

#include "coredefs.h"
#include "dataset.h"
#include <QVector>
#include <QFileInfo>

namespace core {

class QSHARED_EXPORT File final {
public:
  File();
  File(rcstr fileName);

  str name()                 const { return info.fileName(); }
  QFileInfo const& getInfo() const { return info;            }

  void load() THROWS;

private:
  QFileInfo info;
  Datasets  datasets;
};

class QSHARED_EXPORT Files final: public QVector<File*> {
public:
  Files();
 ~Files();
};

}

#endif
