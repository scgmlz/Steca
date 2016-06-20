// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      gui_helpers.h
//! @brief     Helpful utilities for making Gui.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef GUI_HELPERS_H
#define GUI_HELPERS_H

#include "types/core_defs.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListView>
#include <QRadioButton>
#include <QSpinBox>
#include <QToolButton>
#include <QTreeView>

//------------------------------------------------------------------------------

// make connects shorter
#define slot(Type,method,parType) static_cast<void (Type::*)(parType)>(&Type::method)

//------------------------------------------------------------------------------
// handy functions that make (new) widgets

QBoxLayout* boxLayout(Qt::Orientation);
QBoxLayout* hbox();  ///< horizontal box layout
QBoxLayout* vbox();  ///< vertical box layout

QGridLayout* gridLayout();

QLabel*    icon(rcstr);
QLabel*    label(rcstr);
QLineEdit* editCell(uint emWidth);  ///< emWidth: measured in typographical (m)s
QLineEdit* readCell(uint emWidth);
QSpinBox*  spinCell(uint emWidth, int min, int max = INT_MIN);
QDoubleSpinBox* spinCell(uint emWidth, qreal min, qreal max = INT_MIN);
QCheckBox* check(rcstr text, QAction* = nullptr);

QToolButton* textButton(QAction*);
QToolButton* iconButton(QAction*);

QRadioButton* radioButton(rcstr text);

QComboBox* comboBox(str_lst const&);

//------------------------------------------------------------------------------
/// abstract tree widget

class TreeView : public QTreeView {
  Q_OBJECT
  SUPER(TreeView, QTreeView)
public:
  TreeView();

  int sizeHintForColumn(int) const;  // make narrow columns
};

//------------------------------------------------------------------------------
/// abstract tree widget used as a list (hides column 0)

class TreeListView : public TreeView {
  Q_OBJECT
  SUPER(TreeListView, TreeView)
public:
  TreeListView();

protected:
  void setModel(QAbstractItemModel*);
};

//------------------------------------------------------------------------------

class LineView : public QLineEdit {
  SUPER(LineView, QLineEdit)
public:
  LineView();

  void setText(rcstr);
};

//------------------------------------------------------------------------------
/// a widget with a box layout

class BoxWidget : public QWidget {
  SUPER(BoxWidget, QWidget)
public:
  BoxWidget(Qt::Orientation);

protected:
  QBoxLayout* box_;
};

//------------------------------------------------------------------------------
/// a dock widget that acts as BoxWidget

class DockWidget : public QDockWidget {
  SUPER(DockWidget, QDockWidget)
public:
  DockWidget(rcstr name, rcstr objectName, Qt::Orientation);

protected:
  QBoxLayout* box_;
};

//------------------------------------------------------------------------------
#endif  // GUI_HELPERS_H
