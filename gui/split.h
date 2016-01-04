#ifndef SPLIT_H
#define SPLIT_H

#include "gui_helpers.h"

class MainWin;

class Split: public QWidget {
public:
  Split(Qt::Orientation);

protected:
  QBoxLayout *box;
};

#endif
