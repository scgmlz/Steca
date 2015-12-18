#ifndef PANE_H
#define PANE_H

#include "defs.h"
#include <QGroupBox>

class QHBoxLayout; class QVBoxLayout; class QGridLayout;

class Pane: public QGroupBox {
  SUPER(Pane,QGroupBox) Q_OBJECT
public:
  Pane(rcstr title);
  virtual ~Pane();

protected:
  // layout helpers
  typedef QHBoxLayout HBox;
  typedef QVBoxLayout VBox;
  typedef QGridLayout Grid;
};

#endif
