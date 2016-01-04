#ifndef APP_H
#define APP_H

#include "defs.h"
#include <QApplication>

class Core;

// Core + signals
class CoreProxy: public QObject {
  Q_OBJECT
public:
  CoreProxy();
 ~CoreProxy();

  void addFile(rcstr fileName);
  void addFiles(str_lst fileNames);
  bool hasFile(rcstr fileName);
  uint numFiles(bool withCorrection=false);
  str  fileName(uint i);
  void removeFile(uint i);
  bool hasCorrectionFile();
  void setCorrectionFile(rcstr fileName);
  str  correctionFileName();

signals:
  void  filesChanged();

private:
  Core *core;
};

class MainWin;

class App: public QApplication {
  SUPER(App,QApplication)
public:
  App(int& argc, char* argv[]);

  int exec();

  CoreProxy coreProxy;

private:
  MainWin *mainWin;

  bool notify(QObject*, QEvent*);
};

extern App *app;

#endif
