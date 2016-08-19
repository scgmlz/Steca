// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      panel.h
//! @brief     Gui panels (in the main window).
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
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

// Just a plain panel
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

  Tab &addTab(rcstr title, Qt::Orientation = Qt::Vertical);
  Tab &tab(uint);
};

//------------------------------------------------------------------------------
}}
#endif // PANEL_H
