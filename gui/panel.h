#ifndef PANEL_H
#define PANEL_H

#include "defs.h"
#include <QWidget>

class MainWin;
class QVBoxLayout;

class Panel: public QWidget {
public:
  Panel(MainWin&);

  void addGroupBox(rcstr title);

protected:
  MainWin &mainWin;
  QVBoxLayout *v;
};

#endif
