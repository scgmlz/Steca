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
#include <QTreeView>

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

// abstract tree widget
class TreeView: public QTreeView {
  SUPER(TreeView,QTreeView) Q_OBJECT
public:
  TreeView();
};

// abstract tree widget used as a list
class TreeListView: public TreeView {
  SUPER(TreeListView,TreeView) Q_OBJECT
public:
  TreeListView();

protected:
  void setModel(QAbstractItemModel*);
};

#endif
