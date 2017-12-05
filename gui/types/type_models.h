// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/types/type_models.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYPE_MODELS_H
#define TYPE_MODELS_H

#include "refhub.h"
#include <QAbstractTableModel>

class QCheckBox;
class QLineEdit;

namespace models {

extern QVariant const EMPTY_VAR;
extern QModelIndex const ANY_INDEX;

// The base class of all table-like models

class TableModel : public QAbstractTableModel, protected gui::RefHub {
    CLASS(TableModel)
    SUPER(QAbstractTableModel) public : using Index = QModelIndex;
    using rcIndex = Index const&;

    TableModel(gui::TheHub&);

protected:
    // the left-most column is hidden
    static int const DCOL = 1;

public:
    // force-emits reset() signal
    void signalReset();
};
}
#endif // TYPE_MODELS_H
