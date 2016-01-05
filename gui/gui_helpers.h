#ifndef GUI_HELPERS_H
#define GUI_HELPERS_H

#include "defs.h"

#include <QBoxLayout>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QToolButton>
#include <QListView>

QBoxLayout*   hbox();
QBoxLayout*   vbox();
QBoxLayout*   boxLayout(Qt::Orientation);

QGridLayout*  grid(uint margin = 0);

QLabel*       label(rcstr);
QLineEdit*    editCell();
QCheckBox*    check(rcstr text);

QToolButton*  textButton(QAction*);
QToolButton*  iconButton(QAction*);

QRadioButton* radioButton(rcstr text);

// abstract list widget
class ListView: public QListView {
  SUPER(ListView,QListView) Q_OBJECT
public:
  ListView();
};

#endif
