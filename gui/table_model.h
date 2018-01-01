// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/table_model.h
//! @brief     Defines class TableModel
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TABLEMODELS_H
#define TABLEMODELS_H

#include <QAbstractTableModel>

namespace gui {
    class TheHub;
}

namespace models {

extern QVariant const EMPTY_VAR;
extern QModelIndex const ANY_INDEX;

//! The base class of all table-like models

class TableModel : public QAbstractTableModel {
public:
    using Index = QModelIndex;
    using rcIndex = Index const&;

    TableModel() {}

    void signalReset(); //!< force-emits reset() signal

protected:
    static int const DCOL = 1; //!< the left-most column is hidden
};

} // namespace models

#endif // TABLEMODELS_H
