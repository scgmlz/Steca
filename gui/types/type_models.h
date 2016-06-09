// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      type_models.h
//! @brief     Supporting model types.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYPE_MODELS_H
#define TYPE_MODELS_H

#include "refhub.h"
#include "types/core_types_fwd.h"
#include <QAbstractTableModel>

class QCheckBox;
class QLineEdit;

namespace models {

extern QVariant const    EMPTY_VAR;
extern QModelIndex const ANY_INDEX;

//------------------------------------------------------------------------------

struct CheckedInfo {
  str        tag;
  QCheckBox *cb;
  QLineEdit *infoText;

  CheckedInfo(rcstr tag = EMPTY_STR);
  void setText(rcstr);
};

typedef QVector<CheckedInfo> checkedinfo_vec;

//------------------------------------------------------------------------------
/// The base class of all table-like models

class TableModel : public QAbstractTableModel, protected gui::RefHub {
  SUPER(TableModel, QAbstractTableModel)
public:
  using Index   = QModelIndex;
  using rcIndex = Index const&;

  TableModel(gui::TheHub&);

protected:
  /// the left-most column is hidden
  static int const DCOL = 1;

public:
  /// force-emits reset() signal
  void signalReset();
};

//------------------------------------------------------------------------------
}
#endif  // TYPE_MODELS_H
