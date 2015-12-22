#ifndef PANEL_H
#define PANEL_H

#include "defs.h"
#include <QWidget>

class QVBoxLayout;

class Panel: public QWidget {
public:
  Panel();

  void groupBox(rcstr title);

protected:
  QVBoxLayout *v;
};

#endif
