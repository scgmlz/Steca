// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel.h
//! @brief     Gui panels (in the main window).
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef PANEL_H
#define PANEL_H

#include "refhub.h"
#include "gui_helpers.h"
#include <QGroupBox>

class TheHub;

namespace models {
class TableModel;
}

namespace gui { namespace panel {
//------------------------------------------------------------------------------
/// A (tree-)list view with a reference to the hub.

class ListView: public TreeListView, protected RefHub {
  SUPER(ListView,TreeListView)
public:
  ListView(TheHub&);

protected:
  using Model = models::TableModel;
  void updateSingleSelection();
  void clearSelection();
  void selectRow(uint);
  void selectRows(uint_vec);
};

//------------------------------------------------------------------------------

/// Just a plain panel
class BasicPanel: public QGroupBox, protected RefHub {
  SUPER(BasicPanel,QGroupBox)
public:
  BasicPanel(rcstr title,TheHub&);

  void setHorizontalStretch(int);
  void setVerticalStretch(int);
  void setStretch(int horizontal, int vertical);
};

/// A panel with a box layout
class BoxPanel: public BasicPanel {
  SUPER(BoxPanel,BasicPanel)
public:
  BoxPanel(rcstr title, TheHub&, Qt::Orientation);

protected:
  QBoxLayout *box_;
};

/// A panel with grid layout
class GridPanel: public BasicPanel {
  SUPER(GridPanel,BasicPanel)
public:
  GridPanel(rcstr title,TheHub&);

protected:
  QGridLayout *grid_;
};

//------------------------------------------------------------------------------

/// A tabbed panel
class TabsPanel: public QTabWidget, protected RefHub {
  SUPER(TabsPanel,QTabWidget)
public:
  TabsPanel(TheHub&);

protected:
  class Tab: public QWidget {
  public:
    Tab(Qt::Orientation);
    QBoxLayout *box_;
  };

  Tab& addTab(rcstr title,Qt::Orientation);
};

//------------------------------------------------------------------------------
}}
#endif // PANEL_H
