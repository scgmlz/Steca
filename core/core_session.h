#ifndef CORE_SESSION_H
#define CORE_SESSION_H

#include "core_defs.h"
#include "core_file.h"
#include <QSize>
#include <QPointF>

namespace core {

class Session final {
public:
  Session();
 ~Session();

  void addFile(rcstr fileName) THROWS;
  bool hasFile(rcstr fileName);
  void remFile(uint i);

  bool hasCorrFile()   const;
  void setCorrFile(rcstr fileName); // fileName may be empty -> unsets

  Files const& getDataFiles() const { return dataFiles;  }
  File  const& getCorrFile()  const { return *corrFile;  }

  QSize const& getImageSize() const { return imageSize; }

private:
  Files dataFiles;
  QSharedPointer<File> corrFile;

  QSize imageSize;

  void setImageSize(QSize const&) THROWS;
  void setImageSize();
};

}

#endif
