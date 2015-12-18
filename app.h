#ifndef APP_H
#define APP_H

#include "defs.h"
#include <QApplication>

class App: public QApplication {
  SUPER(App,QApplication) Q_OBJECT
public:
  App(int& argc, char* argv[]);

  int run();

protected:
  virtual bool init();
  virtual void done();

protected:
  bool notify(QObject*, QEvent*);
};

#endif
