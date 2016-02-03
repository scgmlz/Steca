/** \file
 */

#ifndef APP_H
#define APP_H

#include "defs.h"
#include <QApplication>

class App: public QApplication {
  SUPER(App,QApplication)
public:
  App(int& argc, char* argv[]);

  int exec();

private:
  bool notify(QObject*, QEvent*);
};

#endif
