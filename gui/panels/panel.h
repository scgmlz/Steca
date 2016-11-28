// ************************************************************************** //
//
// STeCa2 - StressTextureCalculator ver. 2
//
// Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the COPYING and AUTHORS files for more details.
// ************************************************************************** //

#ifndef PANEL_H
#define PANEL_H

#include "gui_helpers.h"
#include "refhub.h"
#include <QGroupBox>

namespace models {
class TableModel;
}

namespace gui { namespace panel {
//------------------------------------------------------------------------------

// REVIEW still needed?

// Just a widget
class PanelWidget : public QWidget, protected RefHub {
  CLS(PanelWidget) SUPER(QWidget)
public:
  PanelWidget(TheHub&, Qt::Orientation);

  QBoxLayout* box() const { return box_; }

protected:
  QBoxLayout *box_;
};

// Just a groupbox
class BasicPanel : public QGroupBox, protected RefHub {
  CLS(BasicPanel) SUPER(QGroupBox)
public:
  BasicPanel(TheHub&);
  BasicPanel(TheHub&, rcstr title);

  void setHorizontalStretch(int);
  void setVerticalStretch(int);
  void setStretch(int horizontal, int vertical);
};

// A panel with a box layout
class BoxPanel : public BasicPanel {
  CLS(BoxPanel) SUPER(BasicPanel)
public:
  BoxPanel(TheHub&, Qt::Orientation);
  BoxPanel(TheHub&, rcstr title, Qt::Orientation);

  QBoxLayout* box() const { return box_; }

protected:
  QBoxLayout *box_;
};

// A panel with grid layout
class GridPanel : public BasicPanel {
  CLS(GridPanel) SUPER(BasicPanel)
public:
  GridPanel(TheHub&);
  GridPanel(TheHub&, rcstr title);

  GridLayout* grid() const { return grid_; }

protected:
  GridLayout *grid_;
};

#ifdef DEVELOP_REBECCA
class FitErrorGridPannel : public GridPanel {
  CLS(FitErrorGridPannel) SUPER(GridPanel)
public:
  using super::super;

  QComboBox         *cbErrorUnits;
  QRadioButton      *rbPercent, *rbAbs;
  QDoubleSpinBox    *spFitError;
};
#endif
//------------------------------------------------------------------------------

// A tabbed panel
class Tab : public QWidget {
  CLS(Tab) SUPER(QWidget)
public:
  Tab(Qt::Orientation);

  QBoxLayout& box() const { return *box_; }

protected:
  QBoxLayout *box_;
};

class TabsPanel : public QTabWidget, protected RefHub {
  CLS(TabsPanel) SUPER(QTabWidget)
public:
  TabsPanel(TheHub&);

  Tab &addTab(rcstr title, Qt::Orientation);
  Tab &tab(uint);
};

//------------------------------------------------------------------------------
}}
#endif // PANEL_H
