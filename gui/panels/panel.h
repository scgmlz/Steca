#ifndef PANEL_H
#define PANEL_H

#include "gui_helpers.h"
#include <QGroupBox>

// abstract panel
class MainWin;
class Session;

class Panel: public QGroupBox {
  SUPER(Panel,QGroupBox)
public:
  Panel(MainWin&,rcstr title,Qt::Orientation);

  void setHorizontalStretch(int);
  void setVerticalStretch(int);
  void setStretch(int horizontal, int vertical);

protected:
  MainWin &mainWin;
  QBoxLayout *box;
};

#endif
