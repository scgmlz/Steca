/** \file
 * Gui panels
 */

#ifndef PANEL_H
#define PANEL_H

#include "gui_helpers.h"
#include <QGroupBox>

class TheHub;

namespace panel {
//------------------------------------------------------------------------------

/// Just a plain panel
class BasicPanel: public QGroupBox {
  SUPER(BasicPanel,QGroupBox)
public:
  BasicPanel(rcstr title,TheHub&);

  void setHorizontalStretch(int);
  void setVerticalStretch(int);
  void setStretch(int horizontal, int vertical);

protected:
  TheHub &theHub;
};

/// A panel with a box layout
class BoxPanel: public BasicPanel {
  SUPER(BoxPanel,BasicPanel)
public:
  BoxPanel(rcstr title,TheHub&,Qt::Orientation);

protected:
  QBoxLayout *box;
};

/// A panel with grid layout
class GridPanel: public BasicPanel {
  SUPER(GridPanel,BasicPanel)
public:
  GridPanel(rcstr title,TheHub&);

protected:
  QGridLayout *grid;
};

/// An information-selection item
struct InfoItem {
  str tag; QCheckBox *cb; QLineEdit *text;
};

typedef QVector<InfoItem> InfoItems;

//------------------------------------------------------------------------------
}
#endif
