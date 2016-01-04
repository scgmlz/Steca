#ifndef GUI_HELPERS_H
#define GUI_HELPERS_H

#include "defs.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QCheckBox>
#include <QRadioButton>

uint const MARGIN = 4;

QHBoxLayout* hbox(uint margin = 0);
QVBoxLayout* vbox(uint margin = 0);
QBoxLayout*  boxLayout(Qt::Orientation, uint margin = 0);

QGridLayout* grid(uint margin = 0);

QLabel* label(rcstr);

int textWidth(rcstr);
int textHeight();

class EditCell: public QLineEdit {
  SUPER(EditCell,QLineEdit)
public:
  EditCell();
};

EditCell* editCell();
QCheckBox* check(rcstr text);

class PushButton: public QPushButton {
  SUPER(PushButton,QPushButton)
public:
  PushButton(rcstr text);
  PushButton(QIcon const&);
  PushButton(QIcon const&, rcstr text);

  void setAction(QAction*);
  // dis/enables the button and its action, thus possibly also a menu item
  void enable(bool);

private:
  QAction *action;
};

PushButton* pushButton(rcstr text);
PushButton* pushButton(QIcon const&);
PushButton* pushButton(QIcon const&, rcstr);

class IconButton: public QToolButton {
  SUPER(IconButton,QToolButton)
public:
  IconButton(QIcon const&);
  IconButton(rcstr iconFile);
};

IconButton* iconButton(QIcon const&);
IconButton* iconButton(rcstr);

QRadioButton* radioButton(rcstr text);

#endif
