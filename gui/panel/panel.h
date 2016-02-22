/** \file
 * Gui panels
 */

#ifndef PANEL_H
#define PANEL_H

#include "gui_helpers.h"
#include <QGroupBox>

class MainWin;
class Session;

namespace panel {

/// Just a plain panel
class BasicPanel: public QGroupBox {
  SUPER(BasicPanel,QGroupBox)
public:
  BasicPanel(rcstr title,MainWin&,Session&);

  void setHorizontalStretch(int);
  void setVerticalStretch(int);
  void setStretch(int horizontal, int vertical);

  Session& getSession() const { return session; }

protected:
  MainWin &mainWin;
  Session &session;
};

/// A panel with a box layout
class BoxPanel: public BasicPanel {
  SUPER(BoxPanel,BasicPanel)
public:
  BoxPanel(rcstr title,MainWin&,Session&,Qt::Orientation);

protected:
  QBoxLayout *box;
};

/// A panel with grid layout
class GridPanel: public BasicPanel {
  SUPER(GridPanel,BasicPanel)
public:
  GridPanel(rcstr title,MainWin&,Session&);

protected:
  QGridLayout *grid;
};

/// An information-selection item
struct InfoItem {
  str tag; QCheckBox *cb; QLineEdit *text;
};

typedef QVector<InfoItem> InfoItems;

}

#endif
