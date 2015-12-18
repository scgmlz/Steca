#ifndef PANE_H
#define PANE_H

#include "defs.h"
#include <QGroupBox>

class Pane: public QGroupBox {
  SUPER(Pane,QGroupBox) Q_OBJECT
public:
  Pane(rcstr title);
  virtual ~Pane();
};

#endif
