// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/table_model.h
//! @brief     Defines class TableModel
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TABLE_MODEL_H
#define TABLE_MODEL_H

#include <QAbstractTableModel>

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

#endif // TABLE_MODEL_H
