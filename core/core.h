#ifndef CORE_H
#define CORE_H

#include <QtCore/qglobal.h>

#ifdef BUILDING_LIBRARY
#define QSHARED_EXPORT Q_DECL_EXPORT
#else
#define QSHARED_EXPORT Q_DECL_IMPORT
#endif

#include <string>
#include <vector>

class QSHARED_EXPORT Core {
public:
  Core();

  typedef std::string        str;
  typedef std::string const& rcstr;

  void  addFile(rcstr fileName);
  uint  numFiles()          const;
  str   fileName(uint i)    const;  // if (i==numFiles()) -> correctionFile()

  bool  hasCorrectionFile() const;
  void  setCorrectionFile(rcstr fileName); // fileName may be empty
  str   correctionFile()    const;

private:
  typedef std::vector<str> str_vec;

  str_vec files;
  str     corrFile;
};

#endif
