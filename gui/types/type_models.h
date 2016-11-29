/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef TYPE_MODELS_H
#define TYPE_MODELS_H

#include "refhub.h"
#include <QAbstractTableModel>

class QCheckBox;
class QLineEdit;

namespace models {

extern QVariant const    EMPTY_VAR;
extern QModelIndex const ANY_INDEX;

//------------------------------------------------------------------------------
// The base class of all table-like models

class TableModel : public QAbstractTableModel, protected gui::RefHub {
  CLS(TableModel) SUPER(QAbstractTableModel)
public:
  using Index   = QModelIndex;
  using rcIndex = Index const&;

  TableModel(gui::TheHub&);

protected:
  // the left-most column is hidden
  static int const DCOL = 1;

public:
  // force-emits reset() signal
  void signalReset();
};

//------------------------------------------------------------------------------
}
#endif // TYPE_MODELS_H
