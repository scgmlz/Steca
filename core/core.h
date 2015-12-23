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
  int   numFiles() const;
  str   fileName(uint i);

private:
  typedef std::vector<str> str_vec;

  str_vec files;
};

#endif
