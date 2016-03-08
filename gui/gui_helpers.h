/** \file
 * Helpful utilities for making Gui.
 */

#ifndef GUI_HELPERS_H
#define GUI_HELPERS_H

#include "core_defs.h"

#include <QBoxLayout>
#include <QDockWidget>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QToolButton>
#include <QListView>
#include <QTreeView>
#include <QComboBox>

//------------------------------------------------------------------------------

QBoxLayout*     hbox();
QBoxLayout*     vbox();
QBoxLayout*     boxLayout(Qt::Orientation);

QGridLayout*    gridLayout();

QLabel*         icon(rcstr);
QLabel*         label(rcstr);
QLineEdit*      editCell(uint emWidth);
QLineEdit*      readCell(uint emWidth);
QSpinBox*       spinCell(uint emWidth,int min,int max = INT_MIN);
QDoubleSpinBox* spinCell(uint emWidth,qreal min,qreal max = INT_MIN);
QCheckBox*      check(rcstr text, QAction* = nullptr);

QToolButton*    textButton(QAction*);
QToolButton*    iconButton(QAction*);

QRadioButton*   radioButton(rcstr text);

QComboBox*      comboBox(QVector<QString> options);

//------------------------------------------------------------------------------
/// abstract list widget

class ListView: public QListView {
  SUPER(ListView,QListView)
  Q_OBJECT
public:
  ListView();
};

//------------------------------------------------------------------------------
/// abstract tree widget

class TreeView: public QTreeView {
  SUPER(TreeView,QTreeView)
  Q_OBJECT
public:
  TreeView();

  int sizeHintForColumn(int) const; // make narrow columns
};

//------------------------------------------------------------------------------
/// abstract tree widget used as a list

class TreeListView: public TreeView {
  SUPER(TreeListView,TreeView)
  Q_OBJECT
public:
  TreeListView();

protected:
  void setModel(QAbstractItemModel*);
};

//------------------------------------------------------------------------------
/// a widget with a box layout

class BoxWidget: public QWidget {
  SUPER(BoxWidget,QWidget)
public:
  BoxWidget(Qt::Orientation);

protected:
  QBoxLayout *box;
};

//------------------------------------------------------------------------------
/// a dock widget that acts as BoxWidget

class DockWidget: public QDockWidget {
  SUPER(DockWidget,QDockWidget)
public:
  DockWidget(rcstr name,rcstr objectName,Qt::Orientation);

protected:
  QBoxLayout *box;
};

//------------------------------------------------------------------------------
#endif
