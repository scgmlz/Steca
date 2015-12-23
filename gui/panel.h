#ifndef PANEL_H
#define PANEL_H

#include "defs.h"
#include <QWidget>
#include <QPushButton>

class MainWin; class Core;
class QVBoxLayout;

// abstract panel

class Panel: public QWidget {
public:
  Panel(MainWin&);

  void addGroupBox(rcstr title);

protected:
  MainWin &mainWin;
  QVBoxLayout *v;
};

// custom widgets

class Button: public QPushButton {
  SUPER(Button,QPushButton)
public:
  Button(rcstr text, rcstr toolTip);
  Button(QIcon const&, rcstr toolTip);
};

class IconButton: public Button {
  SUPER(IconButton,Button)
public:
  IconButton(rcstr fileName, rcstr toolTip);
};

#endif
