#ifndef SESSION_H
#define SESSION_H

#include "defs.h"
#include "core.h"

// Core session + signals
class Session: public QObject {
  Q_OBJECT
public:
  Session();
 ~Session();

  void addFile(rcstr filePath);
  void addFiles(str_lst filePaths);
  bool hasFile(rcstr filePath);
  void remFile(uint i);
  uint numFiles(bool withCorr=false);
  str  fileName(uint i);
  bool hasCorrFile();
  void setCorrFile(rcstr filePath);
  str  corrFileName();

signals:
  void  filesChanged();

private:
  core::Session *coreSession;
};

#endif
