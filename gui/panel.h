#ifndef PANEL_H
#define PANEL_H

#include "gui_helpers.h"
#include <QWidget>
#include <QDockWidget>

// optional dock widget
class Docking: public QDockWidget {
  SUPER(Docking,QDockWidget)
public:
  Docking(rcstr objectName, rcstr windowTitle);
protected:
  QBoxLayout *box;
};

// abstract panel
class MainWin;

class Panel: public QWidget {
public:
  Panel(MainWin&,Qt::Orientation);
  Panel(MainWin&,Qt::Orientation,rcstr groupBoxTitle);

  void setStretchFactors(int horizontal, int vertical);

private:
  Panel(MainWin&,Qt::Orientation,str const* groupBoxTitle);

protected:
  MainWin &mainWin;
  QBoxLayout *box;
};

#endif
