#ifndef EDITCELL_H
#define EDITCELL_H

#include "defs.h"
#include <QLineEdit>

class EditCell: public QLineEdit {
  SUPER(EditCell,QLineEdit)
public:
  EditCell();
};

#endif
