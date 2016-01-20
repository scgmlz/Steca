#ifndef CORE_SESSION_H
#define CORE_SESSION_H

#include "core_defs.h"
#include "core_file.h"
#include <QSize>
#include <QPointF>

namespace core {

class Session {
public:
  Session();
 ~Session();

  void addFile(rcstr fileName) THROWS;
  bool hasFile(rcstr fileName);

  File const& getFile(uint i);
  void remFile(uint i);

  bool hasCorrFile()   const;
  void setCorrFile(rcstr fileName); // fileName may be empty -> unsets

protected:
  Files dataFiles;
  QSharedPointer<File> corrFile;

  QSize imageSize;

  void setImageSize(QSize const&) THROWS;
  void setImageSize();
};

}

#endif
