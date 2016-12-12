/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef GUI_HELPERS_H
#define GUI_HELPERS_H

#include "lib/def/defs.h"
#include "lib/typ/typ_strlst.h"

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

#define slot(Type,method,parType) \
  static_cast<void (Type::*)(parType)>(&Type::method)

//------------------------------------------------------------------------------
// layouts

class GridLayout: public QGridLayout {
  CLS(GridLayout) SUPER(QGridLayout)
public:
  using super::super;

  void addRowStretch(int = 1);
  void addColumnStretch(int = 1);

  static GridLayout* groupBox(QLayout&, rcstr);
};

//------------------------------------------------------------------------------

extern int mWidth(QWidget const*);

//------------------------------------------------------------------------------
// handy functions that make (new) widgets

QBoxLayout* boxLayout(Qt::Orientation);
QBoxLayout* hbox();  // horizontal box layout
QBoxLayout* vbox();  // vertical box layout

GridLayout* gridLayout();

QLabel*    icon(rcstr);
QLabel*    label(rcstr);

QLineEdit* editCell(uint emWidth);  // emWidth: measured in typographical (m)s
QLineEdit* readCell(uint emWidth);

QSpinBox*  spinCell(uint emWidth, int min = INT_MIN, int max = INT_MAX);
QDoubleSpinBox* spinDoubleCell(uint emWidth, qreal min = LLONG_MIN, qreal max = LLONG_MAX);

QCheckBox* check(rcstr text);
QCheckBox* check(QAction* = nullptr);

QToolButton* textButton(QAction*);
QToolButton* iconButton(QAction*);

QRadioButton* radioButton(rcstr text);

QComboBox* comboBox(str_lst::rc);

//------------------------------------------------------------------------------
// abstract tree widget

class TreeView : public QTreeView {
  Q_OBJECT
  CLS(TreeView) SUPER(QTreeView)
public:
  TreeView();

  int sizeHintForColumn(int) const;  // make narrow columns
};

//------------------------------------------------------------------------------
// abstract tree widget used as a list (hides column 0)

class TreeListView : public TreeView {
  Q_OBJECT
  CLS(TreeListView) SUPER(TreeView)
public:
  TreeListView();

protected:
  void setModel(QAbstractItemModel*);
};

//------------------------------------------------------------------------------

class LineView : public QLineEdit {
  CLS(LineView) SUPER(QLineEdit)
public:
  LineView();

  void setText(rcstr);
};

//------------------------------------------------------------------------------
// a widget with a box layout

class BoxWidget : public QWidget {
  CLS(BoxWidget) SUPER(QWidget)
public:
  BoxWidget(Qt::Orientation);

protected:
  QBoxLayout* box_;
};

//------------------------------------------------------------------------------
// a dock widget that acts as BoxWidget

class DockWidget : public QDockWidget {
  CLS(DockWidget) SUPER(QDockWidget)
public:
  DockWidget(rcstr name, rcstr objectName, Qt::Orientation);

protected:
  QBoxLayout* box_;
};

//------------------------------------------------------------------------------
#endif // GUI_HELPERS_H
