#ifndef CORE_H
#define CORE_H

#include <QtCore/qglobal.h>

#ifdef BUILDING_LIBRARY
#define QSHARED_EXPORT Q_DECL_EXPORT
#else
#define QSHARED_EXPORT Q_DECL_IMPORT
#endif

#include <QString>
#include <QVector>
#include <QFileInfo>

class QSHARED_EXPORT Core {
public:
  typedef QString    str;
  typedef str const& rcstr;

  class QSHARED_EXPORT File {
  public:
    File();
    File(rcstr fileName);

    QFileInfo info;
  };

public:
  Core();
 ~Core();

  void addFile(rcstr fileName);
  bool hasFile(rcstr fileName);

  uint numFiles()            const;
  // i between 0..numFiles(); if (i==numFiles()) -> correctionFileName()
  str  fileName(uint i)      const;
  void removeFile(uint i);

  bool hasCorrectionFile()   const;
  void setCorrectionFile(rcstr fileName); // fileName may be empty
  str  correctionFileName()  const;

private:
  typedef QVector<File*> file_vec;

  file_vec dataFiles;
  File     corrFile;
};

#endif
