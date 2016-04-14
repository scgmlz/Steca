// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      gui_helpers.h
//! @brief     Helpful utilities for making Gui.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

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
#include <QFrame>

//------------------------------------------------------------------------------
// handy functions that make (new) widgets

QBoxLayout*     boxLayout(Qt::Orientation);
QBoxLayout*     hbox();       ///< horizontal box layout
QBoxLayout*     vbox();       ///< vertical box layout

QGridLayout*    gridLayout();

QLabel*         icon(rcstr);
QLabel*         label(rcstr);
QLineEdit*      editCell(uint emWidth); ///< emWidth: measured in typographical 'm's
QLineEdit*      readCell(uint emWidth);
QSpinBox*       spinCell(uint emWidth, int   min, int   max = INT_MIN);
QDoubleSpinBox* spinCell(uint emWidth, qreal min, qreal max = INT_MIN);
QCheckBox*      check(rcstr text, QAction* = nullptr);

QToolButton*    textButton(QAction*);
QToolButton*    iconButton(QAction*);

QRadioButton*   radioButton(rcstr text);

QComboBox*      comboBox(str_lst const&);

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
/// abstract tree widget used as a list (hides column 0)

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
#endif // GUI_HELPERS_H
