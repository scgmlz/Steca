#ifndef GUI_HELPERS_H
#define GUI_HELPERS_H

#include "defs.h"

#include <QBoxLayout>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QToolButton>
#include <QListView>

QBoxLayout*   hbox();
QBoxLayout*   vbox();
QBoxLayout*   boxLayout(Qt::Orientation);

QGridLayout*  gridLayout();

QLabel*       icon(rcstr);
QLabel*       label(rcstr);
QLineEdit*    editCell(uint emWidth = 0);
QLineEdit*    readCell(uint emWidth = 0);
QSpinBox*     spinCell(int min,int max);
QCheckBox*    check(rcstr text, QAction* = nullptr);

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
