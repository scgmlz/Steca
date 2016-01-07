#ifndef CORE_CORE_H
#define CORE_CORE_H

#include "coredefs.h"
#include "file.h"

namespace core {

class QSHARED_EXPORT Session final {
public:
  Session();
 ~Session();

  void addFile(rcstr fileName) THROWS;
  bool hasFile(rcstr fileName);
  void remFile(uint i);

  bool hasCorrFile()   const;
  void setCorrFile(rcstr fileName); // fileName may be empty -> unsets

  Files const& getDataFiles() const { return dataFiles; }
  File  const& getCorrFile()  const { return corrFile; }

private:
  Files dataFiles;
  File  corrFile;
};

}

#endif
