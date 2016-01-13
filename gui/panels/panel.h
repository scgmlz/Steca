#ifndef PANEL_H
#define PANEL_H

#include "gui_helpers.h"
#include <QGroupBox>

// abstract panel
class MainWin;
class Session;

class BasicPanel: public QGroupBox {
  SUPER(BasicPanel,QGroupBox)
public:
  BasicPanel(MainWin&,rcstr title);

  void setHorizontalStretch(int);
  void setVerticalStretch(int);
  void setStretch(int horizontal, int vertical);

protected:
  MainWin &mainWin;
};

class BoxPanel: public BasicPanel {
  SUPER(BoxPanel,BasicPanel)
public:
  BoxPanel(MainWin&,rcstr title,Qt::Orientation);

protected:
  QBoxLayout *box;
};

class GridPanel: public BasicPanel {
  SUPER(GridPanel,BasicPanel)
public:
  GridPanel(MainWin&,rcstr title);

protected:
  QGridLayout *grid;
};

#endif
